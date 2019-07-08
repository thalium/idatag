#include "configuration.hpp"

int idaapi show_menu_configuration_ah_t::activate(action_activation_ctx_t *)
{
	myConfiguration->refresh_configuration();
	myConfiguration->show_menu_configuration();
	return 0;
}

action_state_t idaapi show_menu_configuration_ah_t::update(action_update_ctx_t *)
{
	return AST_ENABLE_ALWAYS;
}

Idatag_configuration::Idatag_configuration() 
{
	this->username = std::string("User");
	this->base = get_imagebase();
	
	if (inf_is_64bit()) {
		this->address_width = 16;
	}
	else this->address_width = 8;

	install_menu_configuration();
	create_menu_configuration();
}

void Idatag_configuration::install_menu_configuration()
{
	static const action_desc_t desc = ACTION_DESC_LITERAL(
          "idatag:configuration",
          "[IDATag] ~C~onfiguration",
          &show_menu_configuration_ah,
          NULL,
          NULL, -1);
    if (!register_action(desc)
       || !attach_action_to_menu("Edit/", "idatag:configuration", SETMENU_APP))
    {
		msg("[IDATag] Failed to register configuration menu in IDA");
    }
}

void Idatag_configuration::uninstall_menu_configuration()
{
	if (!unregister_action("idatag:configuration")
		|| !detach_action_from_menu("Edit/", "idatag:configuration"))
	{
		msg("[IDATag] Failed to unregister configuration menu in IDA");
	}
}

void Idatag_configuration::create_menu_configuration()
{
	this->menu_configuration = new QDialog();
	this->menu_configuration->setModal(true);
	this->menu_lab_path = new QLabel("Export JSON file path : ");
	this->menu_lab_user = new QLabel("Username : ");
	QString qusername = QString::fromStdString(this->username);
	QString qpath = QString::fromStdString(this->path);
	this->menu_line_path = new QLineEdit(qpath);
	this->menu_line_user = new QLineEdit(qusername);
	this->menu_button_ok = new QPushButton("OK");
	this->menu_button_cancel = new QPushButton("Cancel");

	this->layout = new QGridLayout(menu_configuration);
	this->layout->addWidget(this->menu_lab_path, 0, 0);
	this->layout->addWidget(this->menu_lab_user, 1, 0);
	this->layout->addWidget(this->menu_line_path, 0, 1);
	this->layout->addWidget(this->menu_line_user, 1, 1);
	this->layout->addWidget(this->menu_button_ok, 2, 0);
	this->layout->addWidget(this->menu_button_cancel, 2, 1);

	this->menu_configuration->setWindowTitle("[IDATag] Configuration...");

	connect(this->menu_button_ok, &QPushButton::pressed, this, &Idatag_configuration::set_configuration);
	connect(this->menu_button_cancel, &QPushButton::pressed, this, &Idatag_configuration::close_configuration);

	this->menu_configuration->resize(360, 120);
}

void Idatag_configuration::refresh_configuration() 
{
	QString qusername = QString::fromStdString(this->username);
	QString qpath = QString::fromStdString(this->path);
	this->menu_line_path->setText(qpath);
	this->menu_line_user->setText(qusername);
}

void Idatag_configuration::load_configuration()
{
	if (myConfiguration->get_path_configuration().empty())
	{
		char filename[QMAXPATH];
		fs::path current_path = fs::current_path();
		fs::path repository = fs::path("tags");
		fs::path tag_path = current_path /= repository;
		get_root_filename(filename, QMAXPATH - 1);
		fs::path spec_tag_path = tag_path /= filename;

		myConfiguration->set_filename_configuration(filename);
		myConfiguration->set_path_configuration(spec_tag_path.string());
	}
}

void Idatag_configuration::set_configuration()
{
	QString qpath = this->menu_line_path->text();
	QString qusername = this->menu_line_user->text();
	this->path = qpath.toStdString();
	this->username = qusername.toStdString();

	this->menu_configuration->close();
}

void Idatag_configuration::close_configuration()
{
	this->menu_configuration->close();
}

void Idatag_configuration::show_menu_configuration()
{
	this->menu_configuration->show();
}

Idatag_configuration::~Idatag_configuration() 
{
	uninstall_menu_configuration();
}

std::string Idatag_configuration::get_path_configuration() 
{
	return this->path;
}
void Idatag_configuration::set_path_configuration(std::string path) 
{
	this->path = path;
}

std::string Idatag_configuration::get_username_configuration() 
{
	return this->username;
}
void Idatag_configuration::set_username_configuration(std::string username) 
{
	this->username = username;
}

uint64_t Idatag_configuration::get_base_configuration() 
{
	return this->base;
}
void Idatag_configuration::set_base_configuration(uint64_t base) 
{
	this->base = base;
}

uint32_t Idatag_configuration::get_address_width_configuration() 
{
	return this->address_width;
}
void Idatag_configuration::set_address_width_configuration(uint32_t address_width) 
{
	this->address_width = address_width;
}

void Idatag_configuration::set_filename_configuration(const char* filename)
{
	this->filename = std::string(filename);
}

std::string Idatag_configuration::get_filename_configuration()
{
	return this->filename;
}