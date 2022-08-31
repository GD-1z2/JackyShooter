#pragma once

#include <GLFW/glfw3.h>
#include <iostream>
#include <jcs/gui.hpp>

class JSGame;

class Screen {
public:
    explicit Screen(JSGame &game) : game{game} {}

    virtual ~Screen() = default;

    virtual void update() {
        for (auto &obj: gui_objects)
            obj->update();
    }

    virtual void render() {
        for (auto &obj: gui_objects)
            obj->render();
    }

    virtual void onResize() {
        for (auto obj: gui_objects)
            if (obj->resize_handler)
                obj->resize_handler(*obj);
    }

    /**
     * Called when the screen is added or becomes topmost after another screen's
     * removal.
     */
    virtual void onFocus() {}

    virtual void onClick(int button, int action, int mods) {
        try {
            for (auto &obj: gui_objects)
                obj->onClick(button, action, mods);
        } catch (std::runtime_error &e) {}
    }

    virtual bool onKey(int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_TAB && action) {
            if (gui_objects.empty())
                return false;

            if (mods & GLFW_MOD_SHIFT) {
                auto o = std::find(gui_objects.begin(), gui_objects.end(),
                                   focused_object) - 1;
                if (o < gui_objects.begin())
                    o = gui_objects.end() - 1;
                focusObject(*o);
            } else {
                auto o = std::find(gui_objects.begin(), gui_objects.end(),
                                   focused_object) + 1;
                if (o >= gui_objects.end())
                    o = gui_objects.begin();
                focusObject(*o);
            }

            return true;
        } else
            try {
                for (auto &obj: gui_objects)
                    if (obj->onKey(key, scancode, action, mods))
                        return true;
            } catch (std::runtime_error &e) {}

        return false;
    }

    virtual void onChar(uint codepoint) {
        for (auto &obj: gui_objects)
            obj->onChar(codepoint);
    }

    void focusObject(GuiObject *object) {
        if (focused_object)
            focused_object->focused = false;

        if (object)
            object->focused = true;

        focused_object = object;
    }

    JSGame &game;
    std::vector<GuiObject *> gui_objects;
    GuiObject *focused_object{nullptr};
};
