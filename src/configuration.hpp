#pragma once

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <kernwin.hpp>

#include <string>
#include <filesystem>

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>

namespace fs = std::filesystem;

struct show_menu_configuration_ah_t : public action_handler_t
{
	virtual int idaapi activate(action_activation_ctx_t *);
	virtual action_state_t idaapi update(action_update_ctx_t *);
};

class Idatag_configuration : public QObject{
	Q_OBJECT
private:
	std::string path;
	std::string username;
	uint64_t base;
	int address_width;
	std::string filename;

	QDialog* menu_configuration;
	QLabel* menu_lab_path;
	QLabel* menu_lab_user;
	QLineEdit* menu_line_path;
	QLineEdit* menu_line_user;
	QPushButton* menu_button_ok;
	QPushButton* menu_button_cancel;
	QGridLayout* layout;

	show_menu_configuration_ah_t show_menu_configuration_ah;

public:
	Idatag_configuration();
	~Idatag_configuration();

	void install_menu_configuration();
	void uninstall_menu_configuration();

	void create_menu_configuration();
	void show_menu_configuration();

	void load_configuration();
	void refresh_configuration();
	void set_configuration();
	void close_configuration();

	std::string get_path_configuration();
	void set_path_configuration(std::string);

	std::string get_username_configuration();
	void set_username_configuration(std::string);

	uint64_t get_base_configuration();
	void set_base_configuration(uint64_t);

	uint32_t get_address_width_configuration();
	void set_address_width_configuration(uint32_t);

	void set_filename_configuration(const char* filename);
	std::string get_filename_configuration();
};

extern Idatag_configuration* myConfiguration;