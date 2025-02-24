/**************************************************************************/
/*  create_dock.cpp                                                       */
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

#include "create_dock.h"

#include "core/io/config_file.h"
#include "editor/connections_dialog.h"
#include "editor/editor_main_screen.h"
#include "editor/editor_node.h"
#include "editor/themes/editor_scale.h"
#include "scene/gui/texture_rect.h"
#include "scene/resources/image_texture.h"

// Returns if success
bool CreateDock::select_category(StringName category) {
	category_label->set_text(category);

	// Is currently in 3D view
	if (EditorNode::get_singleton()->get_editor_main_screen()->get_selected_index() == EditorMainScreen::EditorTable::EDITOR_3D) {
		int8_t found = -1;
		for (int i = 0; i < category_btns_3d.size(); i++) {
			if (categories_3d[i] == category) {
				found = i;
			}
			category_btns_3d[i]->set_pressed(false);
		}

		if (found >= 0) {
			category_btns_3d[found]->set_pressed(true);
			last_category_3d = category;
			return true;
		}
		WARN_PRINT(vformat("Category: %s is not found in 3D mode", category));
		return false;
	}

	// Is currently in 2D view
	if (EditorNode::get_singleton()->get_editor_main_screen()->get_selected_index() == EditorMainScreen::EditorTable::EDITOR_2D) {
		int8_t found = -1;
		for (int i = 0; i < category_btns_2d.size(); i++) {
			if (categories_2d[i] == category) {
				found = i;
			}
			category_btns_2d[i]->set_pressed(false);
		}

		if (found >= 0) {
			category_btns_2d[found]->set_pressed(true);
			last_category_2d = category;
			return true;
		}
		WARN_PRINT(vformat("Category: %s is not found in 2D mode", category));
		return false;
	}

	return false;
}

void CreateDock::notify_main_screen_changed(const String &screen_name) {
	if (screen_name.to_lower() == "2d") {
		_set_mode(Mode::MODE_2D);
		return;
	}

	if (screen_name.to_lower() == "3d") {
		_set_mode(Mode::MODE_3D);
		return;
	}

	// Disable panel if is anything else
	_set_mode(Mode::MODE_NONE);
	return;
}

void CreateDock::_set_mode(Mode mode) {
	current_mode = mode;

	if (current_mode == Mode::MODE_NONE) {
		for (int i = 0; i < category_btns_2d.size(); i++) {
			category_btns_2d[i]->hide();
		}
		for (int i = 0; i < category_btns_3d.size(); i++) {
			category_btns_3d[i]->hide();
		}

		search_bar->hide();
	}

	if (current_mode == Mode::MODE_2D) {
		for (int i = 0; i < category_btns_2d.size(); i++) {
			category_btns_2d[i]->show();
		}
		for (int i = 0; i < category_btns_3d.size(); i++) {
			category_btns_3d[i]->hide();
		}

		if (!last_category_2d.is_empty()) {
			select_category(last_category_2d);
		} else {
			select_category(CATEGORY_2D_BASIC);
		}
		search_bar->show();
	}

	if (current_mode == Mode::MODE_3D) {
		for (int i = 0; i < category_btns_2d.size(); i++) {
			category_btns_2d[i]->hide();
		}
		for (int i = 0; i < category_btns_3d.size(); i++) {
			category_btns_3d[i]->show();
		}

		if (!last_category_3d.is_empty()) {
			select_category(last_category_3d);
		} else {
			select_category(CATEGORY_3D_BASIC);
		}
		search_bar->show();
	}
}

void CreateDock::_save_layout_to_config(Ref<ConfigFile> p_layout, const String &p_section) const {
	// p_layout->set_value(p_section, "dock_nodes_current_tab", int(groups_button->is_pressed()));
}

void CreateDock::_load_layout_from_config(Ref<ConfigFile> p_layout, const String &p_section) {
	// const int current_tab = p_layout->get_value(p_section, "dock_nodes_current_tab", 0);
	// if (current_tab == 0) {
	// 	show_connections();
	// } else if (current_tab == 1) {
	// 	show_groups();
	// }
}

void CreateDock::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_MOUSE_ENTER: {
			_mouse_entered();
		} break;

		case NOTIFICATION_MOUSE_EXIT: {
			_mouse_exited();
		} break;

		case NOTIFICATION_POSTINITIALIZE: {
		} break;

		case NOTIFICATION_THEME_CHANGED: {
			search_bar->set_right_icon(get_editor_theme_icon(SNAME("Search")));

			// Set icons (2D)
			for (int i = 0; i < category_btns_2d.size(); i++) {
				Ref<Image> img = get_editor_theme_icon(category_icons_2d[categories_2d[i]])->get_image()->duplicate();
				img->convert(Image::Format::FORMAT_LA8); // Desaturate icon
				Ref<Texture2D> tex = ImageTexture::create_from_image(img);
				category_btns_2d[i]->set_button_icon(tex);
			}

			// Set icons (3D)
			for (int i = 0; i < category_btns_3d.size(); i++) {
				Ref<Image> img = get_editor_theme_icon(category_icons_3d[categories_3d[i]])->get_image()->duplicate();
				img->convert(Image::Format::FORMAT_LA8); // Desaturate icon
				Ref<Texture2D> tex = ImageTexture::create_from_image(img);
				category_btns_3d[i]->set_button_icon(tex);
			}

			// TEST - Node panel icons - Basic 3D
			for (int i = 0; i < panels[CATEGORY_3D_BASIC].size(); i++) {
				Panel *panel = panels[CATEGORY_3D_BASIC][i];
				TextureRect *tex_rect = Object::cast_to<TextureRect>(panel->get_child(0)->get_child(0)); // panel->vbox->tex_rect
				if (tex_rect != nullptr) {
					tex_rect->set_texture(get_editor_theme_icon(panel->get_meta("node_name")));
				}
			}

		} break;
	}
}

void CreateDock::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_save_layout_to_config"), &CreateDock::_save_layout_to_config);
	ClassDB::bind_method(D_METHOD("_load_layout_from_config"), &CreateDock::_load_layout_from_config);
}

bool CreateDock::can_drop_data(const Point2 &p_point, const Variant &p_data) const {
	bool drop_override = Control::can_drop_data(p_point, p_data); // In case user wants to drop custom data.
	if (drop_override) {
		return drop_override;
	}
	return true;
}

void CreateDock::drop_data(const Point2 &p_point, const Variant &p_data) {
	Control::drop_data(p_point, p_data);
	// TEST
	print_line("Dropped something at create panel");
	if (p_data.get_type() == Variant::Type::OBJECT) {
		Panel *panel = Object::cast_to<Panel>(p_data.get_validated_object());
		if (panel != nullptr && panel->has_meta("node_name")) {
			print_line("Dropped template %s at create panel", panel->get_meta("node_name"));
		}
	}
	// Deconstruct dragged template here (Not dropped in scene)
}

Button *CreateDock::_create_category_button(StringName category) {
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

Panel *CreateDock::_create_template_panel(StringName node_name) {
	if (node_name.is_empty()) {
		return nullptr;
	}

	Panel *panel = memnew(Panel);
	HBoxContainer *hbox = memnew(HBoxContainer);
	TextureRect *tex_rect = memnew(TextureRect);
	Label *label = memnew(Label);

	panel->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	panel->set_v_size_flags(SizeFlags::SIZE_SHRINK_CENTER);
	panel->set_custom_minimum_size(Size2(0, PANEL_VERTICAL_SIZE));
	panel->set_mouse_filter(Control::MouseFilter::MOUSE_FILTER_PASS); // For parent dock mouse entered, mouse exited to work

	panel->add_child(hbox);
	hbox->set_anchors_preset(Control::LayoutPreset::PRESET_FULL_RECT);

	hbox->add_child(tex_rect);
	tex_rect->set_h_size_flags(SizeFlags::SIZE_SHRINK_BEGIN);
	tex_rect->set_v_size_flags(SizeFlags::SIZE_SHRINK_CENTER);
	tex_rect->set_expand_mode(TextureRect::ExpandMode::EXPAND_FIT_WIDTH);
	tex_rect->set_stretch_mode(TextureRect::StretchMode::STRETCH_KEEP_ASPECT_CENTERED);
	tex_rect->set_custom_minimum_size(Size2(0, PANEL_VERTICAL_SIZE));

	hbox->add_child(label);
	label->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	label->set_v_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	label->set_horizontal_alignment(HorizontalAlignment::HORIZONTAL_ALIGNMENT_LEFT);
	label->set_vertical_alignment(VerticalAlignment::VERTICAL_ALIGNMENT_CENTER);

	label->set_text(node_name);

	panel->set_meta("node_name", node_name);

	// Bind events
	panel->connect(SceneStringName(mouse_entered), callable_mp(this, &CreateDock::_template_panel_mouse_entered).bind(panel));
	panel->connect(SceneStringName(mouse_exited), callable_mp(this, &CreateDock::_template_panel_mouse_exited).bind(panel));

	return panel;
}

void CreateDock::_mouse_entered() {
	print_line("Mouse entered CreateDock");
	// Construct template instance here
}

void CreateDock::_mouse_exited() {
	print_line("Mouse exited CreateDock");
	// Deconstuct template instance here
}

void CreateDock::_template_panel_mouse_entered(Panel *panel) {
	print_line(vformat("hovered template: %s", panel->get_meta("node_name")));
	panel->set_self_modulate(Color(0.9, 0.9, 0.9));
}

void CreateDock::_template_panel_mouse_exited(Panel *panel) {
	panel->set_self_modulate(Color(1.0, 1.0, 1.0));
}

void CreateDock::_template_panel_dragged(Vector2 position, Panel *panel) {
	// TEST
	print_line(vformat("Dragged template: %s", panel->get_meta("node_name")));
	// Set dragged template here
}

CreateDock::CreateDock() {
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
	main_scroll = memnew(ScrollContainer);
	main_vbox = memnew(VBoxContainer);
	search_bar = memnew(LineEdit);
	categories_hbox = memnew(HBoxContainer);
	category_label = memnew(Label);

	add_child(main_scroll);
	main_scroll->set_h_size_flags(SIZE_EXPAND_FILL);
	main_scroll->set_v_size_flags(SIZE_EXPAND_FILL);

	main_scroll->add_child(main_vbox);
	main_vbox->set_h_size_flags(SIZE_EXPAND_FILL);
	main_vbox->set_v_size_flags(SIZE_EXPAND_FILL);

	main_vbox->add_child(search_bar);
	search_bar->set_h_size_flags(SIZE_EXPAND_FILL);
	search_bar->set_v_size_flags(SIZE_SHRINK_CENTER);
	search_bar->set_placeholder("Search");

	main_vbox->add_child(categories_hbox);
	categories_hbox->set_h_size_flags(SIZE_EXPAND_FILL);
	categories_hbox->set_v_size_flags(SIZE_SHRINK_CENTER);
	categories_hbox->set_alignment(BoxContainer::AlignmentMode::ALIGNMENT_CENTER);

	main_vbox->add_child(category_label);
	category_label->set_h_size_flags(SIZE_EXPAND_FILL);
	category_label->set_v_size_flags(SIZE_SHRINK_CENTER);
	category_label->set_vertical_alignment(VerticalAlignment::VERTICAL_ALIGNMENT_CENTER);
	category_label->set_horizontal_alignment(HorizontalAlignment::HORIZONTAL_ALIGNMENT_CENTER);

	// Category buttons 2d
	for (int64_t i = 0; i < categories_2d.size(); i++) {
		Button *btn = _create_category_button(categories_2d[i]);
		btn->connect(SceneStringName(pressed), callable_mp(this, &CreateDock::select_category).bind(categories_2d[i])); // Bind callback
		categories_hbox->add_child(btn);
		category_btns_2d.append(btn);
		btn->hide();
	}

	// Category buttons 3d
	for (int64_t i = 0; i < categories_3d.size(); i++) {
		Button *btn = _create_category_button(categories_3d[i]);
		btn->connect(SceneStringName(pressed), callable_mp(this, &CreateDock::select_category).bind(categories_3d[i])); // Bind callback
		categories_hbox->add_child(btn);
		category_btns_3d.append(btn);
		btn->hide();
	}

	// TEST - Basic 3D list
	Panel *node3d_panel = _create_template_panel("Node3D");
	Panel *mesh3d_panel = _create_template_panel("MeshInstance3D");
	panels[CATEGORY_3D_BASIC].append(node3d_panel);
	panels[CATEGORY_3D_BASIC].append(mesh3d_panel);
	main_vbox->add_child(node3d_panel);
	main_vbox->add_child(mesh3d_panel);

	// TEST - Drag templates
	node3d_panel->set_drag_forwarding(callable_mp(this, &CreateDock::_template_panel_dragged).bind(node3d_panel), Callable(), Callable());
	mesh3d_panel->set_drag_forwarding(callable_mp(this, &CreateDock::_template_panel_dragged).bind(mesh3d_panel), Callable(), Callable());
	return;
}

CreateDock::~CreateDock() {
	singleton = nullptr;
}
