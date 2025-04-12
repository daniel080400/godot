/**************************************************************************/
/*  nodes_dock.cpp                                                         */
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

#include "nodes_dock.h"

#include "core/io/config_file.h"
#include "editor/connections_dialog.h"
#include "editor/themes/editor_scale.h"
#include "scene/resources/image_texture.h"
#include "editor/editor_node.h"
#include "editor/editor_main_screen.h"


// Returns if success
bool NodesDock::select_category(StringName category) {
    category_label->set_text(category);

    // Is currently in 3D view
    if (EditorNode::get_singleton()->get_editor_main_screen()->get_selected_index() == EditorMainScreen::EditorTable::EDITOR_3D) {
        int8_t found = -1;
        for (int i = 0; i < category_btns_3d.size(); i++){
            if (categories_3d[i] == category) {
                found = i;
                category_btns_3d[i]->set_pressed(true);
            } else {
                category_btns_3d[i]->set_pressed(false);
            }
        }

        if (found >= 0) {
            _set_mode(Mode::MODE_3D);
            return true;
        }
    }

    // Is currently in 2D view
    if (EditorNode::get_singleton()->get_editor_main_screen()->get_selected_index() == EditorMainScreen::EditorTable::EDITOR_2D) {
        int8_t found = -1;
        for (int i = 0; i < category_btns_2d.size(); i++){
            if (categories_2d[i] == category) {
                found = i;
                category_btns_2d[i]->set_pressed(true);
            } else {
                category_btns_2d[i]->set_pressed(false);
            }
        }

        if (found >= 0) {
            _set_mode(Mode::MODE_2D);
            return true;
        }
    }

    _set_mode(Mode::MODE_NONE);
    return false;
}

void NodesDock::_set_mode(Mode mode) {
    current_mode = mode;

    if (mode == Mode::MODE_NONE) {
        for (int i = 0; i < category_btns_2d.size(); i++) {
            category_btns_2d[i]->hide();
        }
        for (int i = 0; i < category_btns_3d.size(); i++) {
            category_btns_3d[i]->hide();
        } 
    }

    if (mode == Mode::MODE_2D) {
        for (int i = 0; i < category_btns_2d.size(); i++) {
            category_btns_2d[i]->show();
        }
        for (int i = 0; i < category_btns_3d.size(); i++) {
            category_btns_3d[i]->hide();
        }
    }

    if (mode == Mode::MODE_3D) {
        for (int i = 0; i < category_btns_2d.size(); i++) {
            category_btns_2d[i]->hide();
        }
        for (int i = 0; i < category_btns_3d.size(); i++) {
            category_btns_3d[i]->show();
        }
    }
}

void NodesDock::_save_layout_to_config(Ref<ConfigFile> p_layout, const String &p_section) const {
	// p_layout->set_value(p_section, "dock_nodes_current_tab", int(groups_button->is_pressed()));
}

void NodesDock::_load_layout_from_config(Ref<ConfigFile> p_layout, const String &p_section) {
	// const int current_tab = p_layout->get_value(p_section, "dock_nodes_current_tab", 0);
	// if (current_tab == 0) {
	// 	show_connections();
	// } else if (current_tab == 1) {
	// 	show_groups();
	// }
}

void NodesDock::_notification(int p_what) {
	switch (p_what) {
        case NOTIFICATION_POSTINITIALIZE: {
        } break;

		case NOTIFICATION_THEME_CHANGED: {
            search_box->set_right_icon(get_editor_theme_icon(SNAME("Search")));

            // Set icons (2D)
            for (int i = 0; i < category_btns_2d.size(); i++){
                Ref<Image> img = get_editor_theme_icon(category_icons_2d[categories_2d[i]])->get_image()->duplicate();
                img->convert(Image::Format::FORMAT_LA8); // Desaturate icon
                Ref<Texture2D> tex = ImageTexture::create_from_image(img);
                category_btns_2d[i]->set_button_icon(tex);
            }

            // Set icons (3D)
            for (int i = 0; i < category_btns_3d.size(); i++){
                Ref<Image> img = get_editor_theme_icon(category_icons_3d[categories_3d[i]])->get_image()->duplicate();
                img->convert(Image::Format::FORMAT_LA8); // Desaturate icon
                Ref<Texture2D> tex = ImageTexture::create_from_image(img);
                category_btns_3d[i]->set_button_icon(tex);
            }

		} break;
	}
}

void NodesDock::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_save_layout_to_config"), &NodesDock::_save_layout_to_config);
	ClassDB::bind_method(D_METHOD("_load_layout_from_config"), &NodesDock::_load_layout_from_config);
}

Button* NodesDock::_create_category_button(StringName category) {
    Button *btn = memnew(Button);
    btn->set_theme_type_variation(SceneStringName(FlatButton));
    btn->set_toggle_mode(true);
    btn->set_pressed(false);
    btn->set_custom_minimum_size(Size2(32, 32));
    btn->set_h_size_flags(SIZE_SHRINK_CENTER);
    btn->set_v_size_flags(SIZE_SHRINK_CENTER);
    btn->set_icon_alignment(HorizontalAlignment::HORIZONTAL_ALIGNMENT_CENTER);
    btn->set_expand_icon(true);
    btn->set_clip_text(true);
    btn->set_focus_mode(Control::FocusMode::FOCUS_NONE);
    btn->set_text_alignment(HorizontalAlignment::HORIZONTAL_ALIGNMENT_LEFT);
    btn->set_tooltip_text(TTR(category));
    return btn;
}

NodesDock::NodesDock() {
	singleton = this;
    set_name("Nodes");

    // Categories 2D
    categories_2d.push_back(CATEGORY_2D_BASIC); // Most commonly used nodes
    categories_2d.push_back(CATEGORY_2D_LIGHTING);
    categories_2d.push_back(CATEGORY_2D_VFX);
    categories_2d.push_back(CATEGORY_2D_VOLUMES);
    categories_2d.push_back(CATEGORY_2D_RECENT);

    category_icons_2d[CATEGORY_2D_BASIC] = SNAME("Node");
    category_icons_2d[CATEGORY_2D_LIGHTING] = SNAME("GizmoDirectionalLight");
    category_icons_2d[CATEGORY_2D_VFX] = SNAME("GizmoGPUParticles3D");
    category_icons_2d[CATEGORY_2D_VOLUMES] = SNAME("BoxMesh");
    category_icons_2d[CATEGORY_2D_RECENT] = SNAME("Time");

    // Categories 3D
    categories_3d.push_back(CATEGORY_3D_BASIC); 
    categories_3d.push_back(CATEGORY_3D_LIGHTING);
    categories_3d.push_back(CATEGORY_3D_PRIMITIVES); // MeshInstance3D with pre defined mesh resources + staic collision
    categories_3d.push_back(CATEGORY_3D_VFX);
    categories_3d.push_back(CATEGORY_3D_VOLUMES); // Areas with pre defined shape resources
    categories_3d.push_back(CATEGORY_3D_CSG);
    categories_3d.push_back(CATEGORY_3D_RECENT);

    category_icons_3d[CATEGORY_3D_BASIC] = SNAME("Node");
    category_icons_3d[CATEGORY_3D_LIGHTING] = SNAME("GizmoDirectionalLight");
    category_icons_3d[CATEGORY_3D_PRIMITIVES] = SNAME("BoxShape3D");
    category_icons_3d[CATEGORY_3D_VFX] = SNAME("GizmoGPUParticles3D");
    category_icons_3d[CATEGORY_3D_VOLUMES] = SNAME("BoxMesh");
    category_icons_3d[CATEGORY_3D_CSG] = SNAME("CSGBox3D");
    category_icons_3d[CATEGORY_3D_RECENT] = SNAME("Time");

    // Construct layout
    nodes_scroll = memnew(ScrollContainer);
    nodes_vbox = memnew(VBoxContainer);
    search_box = memnew(LineEdit);
    categories_hbox = memnew(HBoxContainer);
    category_label = memnew(Label);

    add_child(nodes_scroll);
    nodes_scroll->set_h_size_flags(SIZE_EXPAND_FILL);
    nodes_scroll->set_v_size_flags(SIZE_EXPAND_FILL);

    nodes_scroll->add_child(nodes_vbox);
    nodes_vbox->set_h_size_flags(SIZE_EXPAND_FILL);
    nodes_vbox->set_v_size_flags(SIZE_EXPAND_FILL);

    nodes_vbox->add_child(search_box);
    search_box->set_h_size_flags(SIZE_EXPAND_FILL);
    search_box->set_v_size_flags(SIZE_SHRINK_CENTER);
    search_box->set_placeholder("Search");

    nodes_vbox->add_child(categories_hbox);
    categories_hbox->set_h_size_flags(SIZE_EXPAND_FILL);
    categories_hbox->set_v_size_flags(SIZE_SHRINK_CENTER);
    categories_hbox->set_alignment(BoxContainer::AlignmentMode::ALIGNMENT_CENTER);

    nodes_vbox->add_child(category_label);
    category_label->set_h_size_flags(SIZE_EXPAND_FILL);
    category_label->set_v_size_flags(SIZE_SHRINK_CENTER);
    category_label->set_vertical_alignment(VerticalAlignment::VERTICAL_ALIGNMENT_CENTER);
    category_label->set_horizontal_alignment(HorizontalAlignment::HORIZONTAL_ALIGNMENT_CENTER);

    // Category buttons 2d
    for (int64_t i = 0; i < categories_2d.size(); i++) {
        Button* btn = _create_category_button(categories_2d[i]);
        btn->connect(SceneStringName(pressed), callable_mp(this, &NodesDock::select_category).bind(categories_2d[i])); // Bind callback
        categories_hbox->add_child(btn);
        category_btns_2d.append(btn);
        btn->hide();
    }

    // Category buttons 3d
    for (int64_t i = 0; i < categories_3d.size(); i++) {
        Button *btn = _create_category_button(categories_3d[i]);
        btn->connect(SceneStringName(pressed), callable_mp(this, &NodesDock::select_category).bind(categories_3d[i])); // Bind callback
        categories_hbox->add_child(btn);
        category_btns_3d.append(btn);
        btn->hide();
    }

    // Will not work since editor is not initialized now
    if (!select_category(CATEGORY_3D_BASIC)) {
        if (!select_category(CATEGORY_2D_BASIC)) {
            select_category(CATEGORY_NONE);
        }
    }

    // TEST - Force show 3d categories for now
    _set_mode(Mode::MODE_3D);

    // WIP find a way to get notify by editor table index changed (2D / 3D / Script / AssetLib)

    return;
}

NodesDock::~NodesDock() {
	singleton = nullptr;
}


