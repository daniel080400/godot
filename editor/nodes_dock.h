/**************************************************************************/
/*  nodes_dock.h                                                           */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef NODES_DOCK_H
#define NODES_DOCK_H

#include "groups_editor.h"
#include "scene/gui/box_container.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/panel.h"
#include "scene/gui/popup.h"
#include "scene/gui/texture_button.h"
#include "scene/main/window.h"

class ConfigFile;
class ConnectionsDock;

class NodesDock : public VBoxContainer {
	GDCLASS(NodesDock, VBoxContainer);

    enum Mode {
        MODE_NONE,
        MODE_2D,
        MODE_3D,
    };

    const StringName CATEGORY_NONE = SNAME("");

    const StringName CATEGORY_2D_BASIC = SNAME("Basic");
    const StringName CATEGORY_2D_LIGHTING = SNAME("Lighting");
    const StringName CATEGORY_2D_VFX = SNAME("Visual Effects");
    const StringName CATEGORY_2D_VOLUMES = SNAME("Volumes");
    const StringName CATEGORY_2D_RECENT = SNAME("Recent");

    const StringName CATEGORY_3D_BASIC = SNAME("Basic");
    const StringName CATEGORY_3D_LIGHTING = SNAME("Lighting");
    const StringName CATEGORY_3D_PRIMITIVES = SNAME("Primitives");
    const StringName CATEGORY_3D_VFX = SNAME("Visual Effects");
    const StringName CATEGORY_3D_VOLUMES = SNAME("Volumes");
    const StringName CATEGORY_3D_CSG = SNAME("CSG");
    const StringName CATEGORY_3D_RECENT = SNAME("Recent");

    Mode current_mode;

    Vector<StringName> categories_2d;
    Vector<StringName> categories_3d;
    HashMap<StringName, StringName> category_icons_2d;
    HashMap<StringName, StringName> category_icons_3d;
    Vector<Button*> category_btns_2d;
    Vector<Button*> category_btns_3d;

    StringName last_category_2d;
    StringName last_category_3d;

    ScrollContainer *nodes_scroll = nullptr;
    VBoxContainer *nodes_vbox = nullptr;
    LineEdit *search_bar = nullptr;
    HBoxContainer *categories_hbox = nullptr;
    Label *category_label = nullptr;

    Button* _create_category_button(StringName category);

    void _set_mode(Mode mode);

	void _save_layout_to_config(Ref<ConfigFile> p_layout, const String &p_section) const;
	void _load_layout_from_config(Ref<ConfigFile> p_layout, const String &p_section);

private:
	inline static NodesDock *singleton = nullptr;

public:
	static NodesDock *get_singleton() { return singleton; }

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
    bool select_category(StringName category);
    void notify_main_screen_changed(const String &screen_name);

	NodesDock();
	~NodesDock();
};

#endif // NODES_DOCK_H
