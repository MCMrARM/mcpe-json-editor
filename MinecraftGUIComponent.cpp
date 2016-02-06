#include "MinecraftGUIComponent.h"
#include "MinecraftGUIContext.h"
#include "MinecraftJSONParser.h"
#include <QJsonArray>
#include <QRegularExpression>
#include <QDebug>

const MCGUIColor MCGUIColor::WHITE = {1.f, 1.f, 1.f, 1.f};

MCGUIComponent::MCGUIComponent(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, Type type, const MCGUIComponent *base, const QJsonObject &object) :
    mcNamespace(mcNamespace), name(name), type(type) {
    ignored.setJSON(object["ignored"], false);
    if (object["variables"].isObject()) {
        variables.push_back(Variables(object["variables"].toObject()));
    } else if (object["variables"].isArray()) {
        QJsonArray vars = object["variables"].toArray();
        for (QJsonValue const& v : vars) {
            if (v.isObject())
                variables.push_back(Variables(v.toObject()));
        }
    }
    if (object["controls"].isArray()) {
        for (QJsonValue v : object["controls"].toArray()) {
            if (!v.isObject())
                continue;
            QJsonObject o = v.toObject();
            for (auto i = o.begin(); i != o.end(); i++) {
                parser.parseComponent(i.key(), mcNamespace, i->toObject(), [this](MCGUIComponent* component) {
                    if (component != nullptr)
                        controls[component->mcNamespace + "." + component->name] = component;
                });
            }
        }
    }
}

MCGUIComponent::Variables::Variables(const QJsonObject &o) {
    requires = o["requires"].toString("");
    for (auto it = o.begin(); it != o.end(); it++) {
        if (it.key() == "requires")
            continue;
        vars[it.key()] = *it;
    }
}

MCGUIComponent::Type MCGUIComponent::getTypeFromString(const QString &type) {
    if (type == "button")
        return Type::BUTTON;
    if (type == "carousel_label")
        return Type::CAROUSEL_LABEL;
    if (type == "custom")
        return Type::CUSTOM;
    if (type == "edit_box")
        return Type::EDIT_BOX;
    if (type == "grid")
        return Type::GRID;
    if (type == "grid_item")
        return Type::GRID_ITEM;
    if (type == "image")
        return Type::IMAGE;
    if (type == "input_panel")
        return Type::INPUT_PANEL;
    if (type == "label")
        return Type::LABEL;
    if (type == "panel")
        return Type::PANEL;
    if (type == "screen")
        return Type::SCREEN;
    if (type == "scrollbar")
        return Type::SCROLLBAR;
    if (type == "scrollbar_box")
        return Type::SCROLLBAR_BOX;
    if (type == "tab")
        return Type::TAB;
    return Type::UNKNOWN;
}

MCGUIComponent* MCGUIComponent::createComponentOfType(MinecraftJSONParser &parser, Type type, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) {
    switch (type) {
    case Type::BUTTON:
        return new MCGUIComponentButton(parser, mcNamespace, name, base, object);
    case Type::CAROUSEL_LABEL:
        return new MCGUIComponentCarouselLabel(parser, mcNamespace, name, base, object);
    case Type::CUSTOM:
        return new MCGUIComponentCustom(parser, mcNamespace, name, base, object);
    case Type::EDIT_BOX:
        return new MCGUIComponentEditBox(parser, mcNamespace, name, base, object);
    case Type::GRID:
        return new MCGUIComponentGrid(parser, mcNamespace, name, base, object);
    case Type::GRID_ITEM:
        return new MCGUIComponentGridItem(parser, mcNamespace, name, base, object);
    case Type::IMAGE:
        return new MCGUIComponentImage(parser, mcNamespace, name, base, object);
    case Type::INPUT_PANEL:
        return new MCGUIComponentInputPanel(parser, mcNamespace, name, base, object);
    case Type::LABEL:
        return new MCGUIComponentLabel(parser, mcNamespace, name, base, object);
    case Type::PANEL:
        return new MCGUIComponentPanel(parser, mcNamespace, name, base, object);
    case Type::SCREEN:
        return new MCGUIComponentScreen(parser, mcNamespace, name, base, object);
    case Type::TAB:
        return new MCGUIComponentTab(parser, mcNamespace, name, base, object);
    }
    return nullptr;
}

MCGUIBaseControl::MCGUIBaseControl(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) : clipOffset({0.f, 0.f}) {
    MCGUICopyBaseProperties(base, Control);
    visible.setJSON(object["visible"]);
    layer.setJSON(object["layer"]);
    clipsChildren.setJSON(object["clips_children"]);
    clipOffset.setJSON(object["clip_offset"]);
    allowClipping.setJSON(object["allow_clipping"]);
    propertyBag.setJSON(object["property_bag"]);
}

MCGUIBaseButtonComponent::MCGUIBaseButtonComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, ButtonComponent);
    defaultControl.setJSON(object["default_control"]);
    hoverControl.setJSON(object["hover_control"]);
    pressedControl.setJSON(object["pressed_control"]);
}

MCGUIBaseDataBindingComponent::MCGUIBaseDataBindingComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, DataBindingComponent);

}

float MCGUILayoutAxis::get(const MCGUIContext *context) {
   MCGUIComponent* component = nullptr;
   Vec2 componentSize;
   if (context != nullptr && context->componentStack.size() > 0) {
       if (context->componentStack.size() > 1) {
           component = context->componentStack[context->componentStack.size() - 2];
       } else {
           component = context->componentStack.last();
       }
       componentSize = component->calculateSize(context);
   }

   float retVal = 0.f;
   for (Component const& c : components) {
       if (c.unit == Component::Unit::PIXELS) {
           retVal += c.value;
       } else if (c.unit == Component::Unit::PERCENT_X) {
           retVal += (c.value * componentSize.x);
       } else if (c.unit == Component::Unit::PERCENT_Y) {
           retVal += (c.value * componentSize.y);
       }
   }
   return retVal;
}

void MCGUILayoutAxis::set(const QJsonValue &obj) {
    set(obj.toString(""));
}

void MCGUILayoutAxis::set(const QString &str) {
    QString s = str;
    if (s.size() <= 0)
        return;
    s.replace(" ", "");
    QStringList a = s.split(QRegularExpression("(?=\\-|\\+)"));
    components.clear();
    for (QString p : a) {
        if (p.length() <= 0)
            continue;
        int i = p.indexOf("%");
        Component c;
        if (i >= 0) {
            c.value = p.midRef(0, i).toFloat() / 100.f;
            if (p.endsWith("x"))
                c.unit = Component::Unit::PERCENT_X;
            else if (p.endsWith("y"))
                c.unit = Component::Unit::PERCENT_Y;
            else
                c.unit = (axis == Axis::X ? Component::Unit::PERCENT_X : Component::Unit::PERCENT_Y);
        } else {
            if (p.endsWith("px"))
                c.value = p.midRef(0, p.length() - 2).toFloat();
            else
                c.value = p.toFloat();

            c.unit = Component::Unit::PIXELS;
        }
        components.push_back(c);
    }
}

MCGUIBaseLayoutComponent::MCGUIBaseLayoutComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, LayoutComponent);
    anchorFrom.setJSON(object["anchor_from"]);
    anchorTo.setJSON(object["anchor_to"]);
    draggable.setJSON(object["draggable"]);
    followsCursor.setJSON(object["follows_cursor"]);
    offset.setJSON(object["offset"]);
    size.setJSON(object["size"]);
}

MCGUIBaseInputComponent::MCGUIBaseInputComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, InputComponent);
    scrollReport.setJSON(object["scroll_report"]);
    alwaysListenToInput.setJSON(object["always_listen_to_input"]);
    focusEnabled.setJSON(object["focus_enabled"]);
    defaultFocusPrecedence.setJSON(object["default_focus_precedence"]);
    alwaysHandlePointer.setJSON(object["always_handle_pointer"]);
    contentPanel.setJSON(object["content_panel"]);
}

MCGUIBaseSoundComponent::MCGUIBaseSoundComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, SoundComponent);
    soundName.setJSON(object["sound_name"]);
    soundVolume.setJSON(object["sound_volume"]);
    soundPitch.setJSON(object["sound_pitch"]);
}

MCGUIBaseTextComponent::MCGUIBaseTextComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, TextComponent);
    text.setJSON(object["text"]);
    alignment.setJSON(object["alignment"]);
    color.setJSON(object["color"]);
    alpha.setJSON(object["alpha"]);
    shadow.setJSON(object["shadow"]);
    //fontSize.setJSON(object["font_size"]);
    wrap.setJSON(object["wrap"]);
    clip.setJSON(object["clip"]);
    localize.setJSON(object["localize"]);
    runeFont.setJSON(object["rune_font"]);
}

MCGUIBaseCarouselTextComponent::MCGUIBaseCarouselTextComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, CarouselTextComponent);
    text.setJSON(object["text"]);
    alignment.setJSON(object["alignment"]);
    color.setJSON(object["color"]);
    alpha.setJSON(object["alpha"]);
    shadow.setJSON(object["shadow"]);
    //fontSize.setJSON(object["font_size"]);
    wrap.setJSON(object["wrap"]);
    clip.setJSON(object["clip"]);
    localize.setJSON(object["localize"]);
    alwaysRotate.setJSON(object["always_rotate"]);
    rotateSpeed.setJSON(object["rotate_speed"]);
    hoverColor.setJSON(object["hover_color"]);
    hoverAlpha.setJSON(object["hover_alpha"]);
    pressedColor.setJSON(object["pressed_color"]);
    pressedAlpha.setJSON(object["pressed_alpha"]);
}

MCGUIBaseTextEditComponent::MCGUIBaseTextEditComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, TextEditComponent);
    textType.setJSON(object["text_type"]);
    maxLength.setJSON(object["max_length"]);
    enabled.setJSON(object["enabled"]);
    textControl.setJSON(object["text_control"]);
}

MCGUIBaseGridComponent::MCGUIBaseGridComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) : gridDimensions({0, 0}) {
    MCGUICopyBaseProperties(base, GridComponent);
    gridDimensions.setJSON(object["grid_dimensions"]);
    gridDimensionBinding.setJSON(object["grid_dimension_binding"]);
    collectionName.setJSON(object["collection_name"]);
    gridRescalingType.setJSON(object["grid_rescaling_type"]);
    maximumGridItems.setJSON(object["maximum_grid_items"]);
    gridItemTemplate.setJSON(object["grid_item_template"]);
}

MCGUIBaseGridItemComponent::MCGUIBaseGridItemComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) : gridPosition({0, 0}) {
    MCGUICopyBaseProperties(base, GridItemComponent);
    gridPosition.setJSON(object["grid_position"]);
}

MCGUIBaseSpriteComponent::MCGUIBaseSpriteComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, SpriteComponent);
    texture.setJSON(object["texture"]);
    uv.setJSON(object["uv"]);
    uvSize.setJSON(object["uv_size"]);
    alpha.setJSON(object["alpha"]);
    color.setJSON(object["color"]);
    ninesliceSize.setJSON(object["nineslice_size"]);
    tiled.setJSON(object["tiled"]);
    clipDirection.setJSON(object["clip_direction"]);
    clipRatio.setJSON(object["clip_ratio"]);
}

MCGUIBaseCustomRendererComponent::MCGUIBaseCustomRendererComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, CustomRendererComponent);
    renderer.setJSON(object["renderer"]);
}

MCGUIBaseTabComponent::MCGUIBaseTabComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, TabComponent);
    tabGroup.setJSON(object["tab_group"]);
    tabIndex.setJSON(object["tab_index"]);
    tabContent.setJSON(object["tab_content"]);
}

MCGUIComponentButton::MCGUIComponentButton(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::BUTTON, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseButtonComponent(*this, base, object), MCGUIBaseDataBindingComponent(*this, base, object), MCGUIBaseLayoutComponent(*this, base, object), MCGUIBaseInputComponent(*this, base, object), MCGUIBaseSoundComponent(*this, base, object) {
    //
}

MCGUIComponentCarouselLabel::MCGUIComponentCarouselLabel(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::CAROUSEL_LABEL, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseDataBindingComponent(*this, base, object), MCGUIBaseLayoutComponent(*this, base, object), MCGUIBaseInputComponent(*this, base, object), MCGUIBaseCarouselTextComponent(*this, base, object) {
    //
}

MCGUIComponentCustom::MCGUIComponentCustom(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::CUSTOM, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseDataBindingComponent(*this, base, object), MCGUIBaseLayoutComponent(*this, base, object), MCGUIBaseCustomRendererComponent(*this, base, object) {
    //
}

MCGUIComponentEditBox::MCGUIComponentEditBox(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::EDIT_BOX, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseButtonComponent(*this, base, object), MCGUIBaseDataBindingComponent(*this, base, object), MCGUIBaseInputComponent(*this, base, object), MCGUIBaseLayoutComponent(*this, base, object), MCGUIBaseTextEditComponent(*this, base, object) {
    //
}

MCGUIComponentGrid::MCGUIComponentGrid(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::GRID, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseDataBindingComponent(*this, base, object), MCGUIBaseGridComponent(*this, base, object), MCGUIBaseLayoutComponent(*this, base, object) {
    //
}

MCGUIComponentGridItem::MCGUIComponentGridItem(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::GRID_ITEM, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseGridItemComponent(*this, base, object), MCGUIBaseLayoutComponent(*this, base, object) {
    //
}

MCGUIComponentImage::MCGUIComponentImage(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::IMAGE, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseDataBindingComponent(*this, base, object), MCGUIBaseLayoutComponent(*this, base, object), MCGUIBaseSpriteComponent(*this, base, object) {
    //
}

MCGUIComponentInputPanel::MCGUIComponentInputPanel(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::INPUT_PANEL, base, object),
    MCGUIBaseInputComponent(*this, base, object) {
    //
}

MCGUIComponentLabel::MCGUIComponentLabel(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::LABEL, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseDataBindingComponent(*this, base, object), MCGUIBaseLayoutComponent(*this, base, object), MCGUIBaseTextComponent(*this, base, object) {
    //
}

MCGUIComponentPanel::MCGUIComponentPanel(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::PANEL, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseDataBindingComponent(*this, base, object), MCGUIBaseLayoutComponent(*this, base, object) {
    //
}

MCGUIComponentScreen::MCGUIComponentScreen(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::SCREEN, base, object),
    MCGUIBaseDataBindingComponent(*this, base, object), MCGUIBaseLayoutComponent(*this, base, object) {
    //
}

MCGUIComponentTab::MCGUIComponentTab(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(parser, mcNamespace, name, Type::TAB, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseButtonComponent(*this, base, object), MCGUIBaseDataBindingComponent(*this, base, object), MCGUIBaseInputComponent(*this, base, object), MCGUIBaseLayoutComponent(*this, base, object), MCGUIBaseSoundComponent(*this, base, object), MCGUIBaseTabComponent(*this, base, object) {
    //
}
