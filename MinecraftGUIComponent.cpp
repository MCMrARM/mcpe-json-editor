#include "MinecraftGUIComponent.h"
#include "MinecraftGUIContext.h"
#include "MinecraftJSONParser.h"
#include <QJsonArray>
#include <QRegularExpression>
#include <QDebug>

MCGUIComponent::MCGUIComponent(const QString &mcNamespace, const QString &name, const QJsonObject &object) :
    mcNamespace(mcNamespace), name(name) {
    ignored.set(object["ignored"], false);
    if (object["variables"].isObject()) {
        variables.push_back(Variables(object["variables"].toObject()));
    } else if (object["variables"].isArray()) {
        QJsonArray vars = object["variables"].toArray();
        for (QJsonValue const& v : vars) {
            if (v.isObject())
                variables.push_back(Variables(v.toObject()));
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

MCGUIComponent* MCGUIComponent::createComponentOfType(Type type, const QString &mcNamespace, const QString &name, const QJsonObject &object) {
    switch (type) {
    case Type::BUTTON:
        return new MCGUIComponentButton(mcNamespace, name, object);
    case Type::PANEL:
        return new MCGUIComponentPanel(mcNamespace, name, object);
    }
    return nullptr;
}

MCGUIControl::MCGUIControl(const MCGUIComponent &component, const QJsonObject &object) {
    visible.set(object["visible"], true);
    layer.set(object["layer"], 0);
    clipsChildren.set(object["clips_children"], false);
    clipOffset.set(object["clip_offset"], {0.f, 0.f});
    allowClipping.set(object["allow_clipping"], true);
    propertyBag.set(object["property_bag"], QJsonObject());
}

MCGUIButtonControl::MCGUIButtonControl(const MCGUIComponent &component, const QJsonObject &object) {
    defaultControl.set(object["default_control"], {""});
    hoverControl.set(object["hover_control"], {""});
    pressedControl.set(object["pressed_control"], {""});
}

MCGUIDataBindingControl::MCGUIDataBindingControl(const MCGUIComponent &component, const QJsonObject &object) {

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

MCGUILayoutControl::MCGUILayoutControl(const MCGUIComponent &component, const QJsonObject &object) {
    anchorFrom.set(object["anchor_from"], MCGUIAnchorPoint::CENTER);
    anchorTo.set(object["anchor_to"], MCGUIAnchorPoint::CENTER);
    draggable.set(object["draggable"], MCGUIDraggable::NOT_DRAGGABLE);
    followsCursor.set(object["follows_cursor"], false);
    offset.set(object["offset"], MCGUILayoutOffset());
    size.set(object["size"], MCGUILayoutOffset());
}

MCGUIComponentButton::MCGUIComponentButton(const QString &mcNamespace, const QString &name, const QJsonObject &object) :
    MCGUIComponent(mcNamespace, name, object),
    MCGUIControl(*this, object), MCGUIButtonControl(*this, object), MCGUIDataBindingControl(*this, object), MCGUILayoutControl(*this, object) {
    //
}

MCGUIComponentPanel::MCGUIComponentPanel(const QString &mcNamespace, const QString &name, const QJsonObject &object) :
    MCGUIComponent(mcNamespace, name, object),
    MCGUIControl(*this, object), MCGUIDataBindingControl(*this, object), MCGUILayoutControl(*this, object) {
    //
}

