#include "MinecraftGUIComponent.h"
#include "MinecraftGUIContext.h"
#include "MinecraftJSONParser.h"
#include <QJsonArray>
#include <QRegularExpression>

const MCGUIColor MCGUIColor::WHITE = {1.f, 1.f, 1.f, 1.f};

#define MCGUIProperty(name, jsonName) name.setJSON(object, #jsonName, component.bindings);

MCGUIComponent::MCGUIComponent(const QString &mcNamespace, const QString &name, Type type, bool parseDataBindings, const MCGUIComponent *base, const QJsonObject &object) :
    mcNamespace(mcNamespace), name(name), type(type) {
    if (base != nullptr) {
        ignored = base->ignored;
        bindings = base->bindings;
        variables = base->variables;
        controls = base->controls;
    }
    {
        Variables vars;
        for (auto i = object.begin(); i != object.end(); i++) {
            if (i.key()[0] == '$')
                vars.vars[i.key()] = *i;
        }
        variables.push_back(vars);
    }
    if (object["variables"].isObject()) {
        variables.push_back(Variables(object["variables"].toObject()));
    } else if (object["variables"].isArray()) {
        QJsonArray vars = object["variables"].toArray();
        for (QJsonValue const& v : vars) {
            if (v.isObject())
                variables.push_back(Variables(v.toObject()));
        }
    }
    if (parseDataBindings) {
        for (QJsonValue v : object["bindings"].toArray()) {
            if (!v.isObject())
                continue;

            QJsonObject o = v.toObject();
            MCGUIDataBinding binding;
            binding.name.setJSON(o["binding_name"], "");
            binding.nameOverride.setJSON(o["binding_name_override"], "");
            binding.type.setJSON(o["binding_type"], MCGUIDataBinding::Type::GLOBAL);
            bindings.push_back(binding);
        }
    }
    ignored.setJSON(object, "ignored", bindings);
    if (object["controls"].isArray()) {
        for (QJsonValue v : object["controls"].toArray()) {
            if (!v.isObject())
                continue;

            QJsonObject o = v.toObject();
            for (auto i = o.begin(); i != o.end(); i++) {
                QJsonObject io = i->toObject();
                MCGUIVariableExtendComponent el;
                QString cName = i.key();
                if (!cName.contains("@") && !cName.startsWith("$")) {
                    MCGUIComponent::Type type = MCGUIComponent::getTypeFromString(io["type"].toString(""));
                    el.name = cName;
                    el.component = MCGUIComponent::createComponentOfType(type, mcNamespace, cName, nullptr, io);
                    if (el.component != nullptr)
                        controls.push_back(el);
                    continue;
                }
                el.object = io;
                el.mcNamespace = mcNamespace;
                el.name = cName;
                el.name.variableName = cName;
                controls.push_back(el);
            }
        }
    }
}

MCGUIComponent *MCGUIVariableExtendComponent::get(MCGUIContext *context) {
    if (component != nullptr)
        return component;
    if (context == nullptr)
        return nullptr;
    QString fName = name.get(context);
    if (components.contains(fName))
        return components[fName];
    int i = fName.indexOf("@");
    MCGUIComponent::Type type = MCGUIComponent::getTypeFromString(object["type"].toString(""));
    if (i < 0) {
        MCGUIComponent *c = MCGUIComponent::createComponentOfType(type, mcNamespace, fName, nullptr, object);
        components[fName] = c;
        return c;
    }
    MCGUIVariable<QString> _eName (fName.mid(i + 1));
    QString eName = _eName.get(context);
    QString name = fName.mid(0, i);
    if (!eName.contains("."))
        eName = mcNamespace + "." + eName;
    if (context->components.contains(eName)) {
        MCGUIComponent *extendComponent = context->components[eName];
        if (extendComponent == nullptr)
            return nullptr;
        if (type == MCGUIComponent::Type::UNKNOWN)
            type = extendComponent->type;
        MCGUIComponent *c = MCGUIComponent::createComponentOfType(type, mcNamespace, name, extendComponent, object);
        components[fName] = c;
        return c;
    }
    return nullptr;
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

MCGUIComponent* MCGUIComponent::createComponentOfType(Type type, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject object) {
    if (base != nullptr && base->type == Type::UNKNOWN) {
        const QJsonObject& baseObject = ((MCGUIUnknownComponent*) base)->object;
        for (auto it = baseObject.begin(); it != baseObject.end(); it++) {
            if (!object.contains(it.key()))
                object[it.key()] = *it;
        }
    }

    switch (type) {
    case Type::BUTTON:
        return new MCGUIComponentButton(mcNamespace, name, base, object);
    case Type::CAROUSEL_LABEL:
        return new MCGUIComponentCarouselLabel(mcNamespace, name, base, object);
    case Type::CUSTOM:
        return new MCGUIComponentCustom(mcNamespace, name, base, object);
    case Type::EDIT_BOX:
        return new MCGUIComponentEditBox(mcNamespace, name, base, object);
    case Type::GRID:
        return new MCGUIComponentGrid(mcNamespace, name, base, object);
    case Type::GRID_ITEM:
        return new MCGUIComponentGridItem(mcNamespace, name, base, object);
    case Type::IMAGE:
        return new MCGUIComponentImage(mcNamespace, name, base, object);
    case Type::INPUT_PANEL:
        return new MCGUIComponentInputPanel(mcNamespace, name, base, object);
    case Type::LABEL:
        return new MCGUIComponentLabel(mcNamespace, name, base, object);
    case Type::PANEL:
        return new MCGUIComponentPanel(mcNamespace, name, base, object);
    case Type::SCREEN:
        return new MCGUIComponentScreen(mcNamespace, name, base, object);
    case Type::SCROLLBAR:
        return new MCGUIComponentScrollbar(mcNamespace, name, base, object);
    case Type::SCROLLBAR_BOX:
        return new MCGUIComponentScrollbarBox(mcNamespace, name, base, object);
    case Type::TAB:
        return new MCGUIComponentTab(mcNamespace, name, base, object);
    case Type::UNKNOWN:
        return new MCGUIUnknownComponent(mcNamespace, name, base, object);
    }
    return nullptr;
}

MCGUIBaseControl::MCGUIBaseControl(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) : clipOffset({0.f, 0.f}) {
    MCGUICopyBaseProperties(base, Control);
    MCGUIProperty(visible, visible);
    MCGUIProperty(layer, layer);
    MCGUIProperty(clipsChildren, clips_children);
    MCGUIProperty(clipOffset, clip_offset);
    MCGUIProperty(allowClipping, allow_clipping);
    MCGUIProperty(propertyBag, property_bag);
}

MCGUIBaseButtonComponent::MCGUIBaseButtonComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, ButtonComponent);
    MCGUIProperty(defaultControl, default_control);
    MCGUIProperty(hoverControl, hover_control);
    MCGUIProperty(pressedControl, pressed_control);
}

float MCGUILayoutAxis::get(Vec2 componentSize) {
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

float MCGUILayoutAxis::get(MCGUIContext *context) {
   return get(context->getParentComponentSize());
}

void MCGUILayoutAxis::set(const QJsonValue &obj) {
    if (obj.isDouble()) {
        components.clear();
        components.push_back({Component::Unit::PIXELS, (float) obj.toDouble()});
        return;
    }
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

Vec2 MCGUIGetAnchorPoint(Vec2 size, MCGUIAnchorPoint point) {
    switch (point) {
    case MCGUIAnchorPoint::TOP_LEFT:
        return {0.f, 0.f};
    case MCGUIAnchorPoint::TOP_MIDDLE:
        return {size.x / 2, 0.f};
    case MCGUIAnchorPoint::TOP_RIGHT:
        return {size.x, 0.f};
    case MCGUIAnchorPoint::LEFT_MIDDLE:
        return {0.f, size.y / 2};
    case MCGUIAnchorPoint::CENTER:
        return {size.x / 2, size.y / 2};
    case MCGUIAnchorPoint::RIGHT_MIDDLE:
        return {size.x, size.y / 2};
    case MCGUIAnchorPoint::BOTTOM_LEFT:
        return {0.f, size.y};
    case MCGUIAnchorPoint::BOTTOM_MIDDLE:
        return {size.x / 2, size.y};
    case MCGUIAnchorPoint::BOTTOM_RIGHT:
        return {size.x, size.y};
    }
    return {0.f, 0.f};
}

Vec2 MCGUILayoutComponent::getPos(MCGUIContext *context) {
    Vec2 parentSize = context->getParentComponentSize();
    Vec2 off = offset.get(context).get(parentSize);
    Vec2 size = calculateSize(context);
    Vec2 parentAnchorPos = MCGUIGetAnchorPoint(parentSize, anchorFrom.get(context));
    Vec2 myAnchorPos = MCGUIGetAnchorPoint(size, anchorTo.get(context));
    return parentAnchorPos - myAnchorPos + off;
}

MCGUIBaseLayoutComponent::MCGUIBaseLayoutComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUILayoutOffset off;
    off.x.components.push_back({MCGUILayoutAxis::Component::Unit::PERCENT_X, 1.f});
    off.y.components.push_back({MCGUILayoutAxis::Component::Unit::PERCENT_Y, 1.f});
    size.set(off);
    MCGUICopyBaseProperties(base, LayoutComponent);
    MCGUIProperty(anchorFrom, anchor_from);
    MCGUIProperty(anchorTo, anchor_to);
    MCGUIProperty(draggable, draggable);
    MCGUIProperty(followsCursor, follows_cursor);
    MCGUIProperty(offset, offset);
    MCGUIProperty(size, size);
}

MCGUIBaseInputComponent::MCGUIBaseInputComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, InputComponent);
    MCGUIProperty(scrollReport, scroll_report);
    MCGUIProperty(alwaysListenToInput, always_listen_to_input);
    MCGUIProperty(focusEnabled, focus_enabled);
    MCGUIProperty(defaultFocusPrecedence, default_focus_precedence);
    MCGUIProperty(alwaysHandlePointer, always_handle_pointer);
    MCGUIProperty(contentPanel, content_panel);
}

MCGUIBaseSoundComponent::MCGUIBaseSoundComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, SoundComponent);
    MCGUIProperty(soundName, sound_name);
    MCGUIProperty(soundVolume, sound_volume);
    MCGUIProperty(soundPitch, sound_pitch);
}

MCGUIBaseTextComponent::MCGUIBaseTextComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, TextComponent);
    MCGUIProperty(text, text);
    MCGUIProperty(alignment, alignment);
    MCGUIProperty(color, color);
    MCGUIProperty(alpha, alpha);
    MCGUIProperty(shadow, shadow);
    //MCGUIProperty(fontSize, font_size);
    MCGUIProperty(wrap, wrap);
    MCGUIProperty(clip, clip);
    MCGUIProperty(localize, localize);
    MCGUIProperty(runeFont, rune_font);
}

MCGUIBaseCarouselTextComponent::MCGUIBaseCarouselTextComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, CarouselTextComponent);
    MCGUIProperty(text, text);
    MCGUIProperty(alignment, alignment);
    MCGUIProperty(color, color);
    MCGUIProperty(alpha, alpha);
    MCGUIProperty(shadow, shadow);
    //MCGUIProperty(fontSize, font_size);
    MCGUIProperty(wrap, wrap);
    MCGUIProperty(clip, clip);
    MCGUIProperty(localize, localize);
    MCGUIProperty(alwaysRotate, always_rotate);
    MCGUIProperty(rotateSpeed, rotate_speed);
    MCGUIProperty(hoverColor, hover_color);
    MCGUIProperty(hoverAlpha, hover_alpha);
    MCGUIProperty(pressedColor, pressed_color);
    MCGUIProperty(pressedAlpha, pressed_alpha);
}

MCGUIBaseTextEditComponent::MCGUIBaseTextEditComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, TextEditComponent);
    MCGUIProperty(textType, text_type);
    MCGUIProperty(maxLength, max_length);
    MCGUIProperty(enabled, enabled);
    MCGUIProperty(textControl, text_control);
}

MCGUIBaseGridComponent::MCGUIBaseGridComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) : gridDimensions({0, 0}) {
    MCGUICopyBaseProperties(base, GridComponent);
    MCGUIProperty(gridDimensions, grid_dimensions);
    MCGUIProperty(gridDimensionBinding, grid_dimension_binding);
    MCGUIProperty(collectionName, collection_name);
    MCGUIProperty(gridRescalingType, grid_rescaling_type);
    MCGUIProperty(maximumGridItems, maximum_grid_items);
    MCGUIProperty(gridItemTemplate, grid_item_template);
}

MCGUIBaseGridItemComponent::MCGUIBaseGridItemComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) : gridPosition({0, 0}) {
    MCGUICopyBaseProperties(base, GridItemComponent);
    MCGUIProperty(gridPosition, grid_position);
}

MCGUIBaseSpriteComponent::MCGUIBaseSpriteComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) : uv({0.f, 0.f}), uvSize({0.f, 0.f}), ninesliceSize({0.f, 0.f, 0.f, 0.f}) {
    MCGUICopyBaseProperties(base, SpriteComponent);
    MCGUIProperty(texture, texture);
    MCGUIProperty(uv, uv);
    MCGUIProperty(uvSize, uv_size);
    MCGUIProperty(alpha, alpha);
    MCGUIProperty(color, color);
    MCGUIProperty(ninesliceSize, nineslice_size);
    MCGUIProperty(tiled, tiled);
    MCGUIProperty(clipDirection, clip_direction);
    MCGUIProperty(clipRatio, clip_ratio);
}

MCGUIBaseCustomRendererComponent::MCGUIBaseCustomRendererComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, CustomRendererComponent);
    MCGUIProperty(renderer, renderer);
}

MCGUIBaseScrollbarComponent::MCGUIBaseScrollbarComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, ScrollbarComponent);
    MCGUIProperty(scrollbarBoxTrackButton, scrollbar_box_track_button);
    MCGUIProperty(scrollbarTouchButton, scrollbar_touch_button);
    MCGUIProperty(dampening, dampening);
    MCGUIProperty(scrollSpeed, scroll_speed);
    MCGUIProperty(scrollbarBox, scrollbar_box);
}

MCGUIBaseTabComponent::MCGUIBaseTabComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object) {
    MCGUICopyBaseProperties(base, TabComponent);
    MCGUIProperty(tabGroup, tab_group);
    MCGUIProperty(tabIndex, tab_index);
    MCGUIProperty(tabContent, tab_content);
}

MCGUIComponentButton::MCGUIComponentButton(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::BUTTON, true, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseButtonComponent(*this, base, object), MCGUIBaseInputComponent(*this, base, object), MCGUIBaseSoundComponent(*this, base, object) {
    //
}

MCGUIComponentCarouselLabel::MCGUIComponentCarouselLabel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::CAROUSEL_LABEL, true, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseInputComponent(*this, base, object), MCGUIBaseCarouselTextComponent(*this, base, object) {
    //
}

MCGUIComponentCustom::MCGUIComponentCustom(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::CUSTOM, true, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseCustomRendererComponent(*this, base, object) {
    //
}

MCGUIComponentEditBox::MCGUIComponentEditBox(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::EDIT_BOX, true, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseButtonComponent(*this, base, object), MCGUIBaseInputComponent(*this, base, object), MCGUIBaseTextEditComponent(*this, base, object) {
    //
}

MCGUIComponentGrid::MCGUIComponentGrid(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::GRID, true, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseGridComponent(*this, base, object) {
    //
}

MCGUIComponentGridItem::MCGUIComponentGridItem(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::GRID_ITEM, false, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseGridItemComponent(*this, base, object) {
    //
}

MCGUIComponentImage::MCGUIComponentImage(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::IMAGE, true, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseSpriteComponent(*this, base, object) {
    //
}

MCGUIComponentInputPanel::MCGUIComponentInputPanel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::INPUT_PANEL, true, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseInputComponent(*this, base, object) {
    //
}

MCGUIComponentLabel::MCGUIComponentLabel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::LABEL, true, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseTextComponent(*this, base, object) {
    //
}

MCGUIComponentPanel::MCGUIComponentPanel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::PANEL, true, base, object),
    MCGUIBaseControl(*this, base, object) {
    //
}

MCGUIComponentScreen::MCGUIComponentScreen(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(mcNamespace, name, Type::SCREEN, true, base, object) {
    //
}

MCGUIComponentScrollbar::MCGUIComponentScrollbar(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::SCROLLBAR, false, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseInputComponent(*this, base, object), MCGUIBaseScrollbarComponent(*this, base, object) {
    //
}

MCGUIComponentScrollbarBox::MCGUIComponentScrollbarBox(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::SCROLLBAR_BOX, false, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseInputComponent(*this, base, object) {
    //
}

MCGUIComponentTab::MCGUIComponentTab(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUILayoutComponent(mcNamespace, name, Type::TAB, true, base, object),
    MCGUIBaseControl(*this, base, object), MCGUIBaseButtonComponent(*this, base, object), MCGUIBaseInputComponent(*this, base, object), MCGUIBaseSoundComponent(*this, base, object), MCGUIBaseTabComponent(*this, base, object) {
    //
}

MCGUIUnknownComponent::MCGUIUnknownComponent(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object) :
    MCGUIComponent(mcNamespace, name, Type::UNKNOWN, false, base, object), object(object) {
    //
}
