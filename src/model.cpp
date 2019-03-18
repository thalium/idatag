#include "model.hpp"

using json = nlohmann::json;


bool check_rva(uint64_t rva)
{
	return is_mapped(rva);
}

void Idatag_model::init_model() 
{
	import_info_disas();
	import_tags();
	print_stats_model();
}

int Idatag_model::count_stats_tags()
{
	int count = 0;
	for (std::vector<Offset>::iterator it = mydata.begin(); it != mydata.end(); ++it) {
		count += (*it).count_tags();
	}
	return count;
}

void Idatag_model::print_stats_model() 
{
	size_t nb_tags = count_stats_tags();
	size_t nb_offsets = mydata.size();
	msg("[IDATag] Tags : %d on %d offsets!", nb_tags, nb_offsets);
}

std::vector<Offset> Idatag_model::get_data() const 
{
	return mydata;
}

std::vector<std::string> Idatag_model::get_feeders() const 
{
	return myfeeders;
}

Qt::ItemFlags Idatag_model::flags(const QModelIndex &index) const 
{
	if (index.column() == 2) {
		return Qt::ItemIsEditable | Qt::ItemIsEnabled;
	}
	return Qt::ItemIsEnabled;
}

int Idatag_model::rowCount(const QModelIndex &parent) const 
{
	return mydata.size();
}

int Idatag_model::columnCount(const QModelIndex &parent) const 
{
	return 3;
}

QVariant Idatag_model::data(const QModelIndex &index, int role) const
{
	uint64_t rva;
	std::string name;
	QString qstr_name;

	if (!index.isValid()) {
		return QVariant();
	}

	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	int row = index.row();
	int column = index.column();

	if(row > rowCount()) {
		return QVariant();
	}
	if (column > columnCount()) {
		return QVariant();
	}

	const Offset* offset = get_offset_byindex(row);

	switch(column) 
	{
		case 0:
			rva = offset->get_rva();
			return (QVariant)rva;
		case 1:
			name = offset->get_name();
			qstr_name = QString::fromStdString(name);
			return qstr_name;
		case 2:
			return "tags";
	}
	return QVariant();
}

//bool Idatag_model::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {}
//bool Idatag_model::insertRows(int position, int rows, const QModelIndex $index = QModelIndex()) {}
//bool Idatag_model::removeRows(int position, int rows, const QModelIndex $index = QModelIndex()) {}

void Idatag_model::add_offset(uint64_t rva) 
{
	Offset offset(rva);
	mydata.push_back(offset);
}

void Idatag_model::add_offset(Offset& offset) 
{
	mydata.push_back(offset);
}

Offset* Idatag_model::get_offset_byrva(uint64_t rva)
{
	if (!check_rva(rva)) {
		return NULL;
	}
	for (std::vector<Offset>::iterator it = mydata.begin(); it != mydata.end(); ++it) {
		if (it->get_rva() == rva) {
			return &(*it);
		}
	}

	Offset offset = Offset(rva);
	add_offset(offset);

	for (std::vector<Offset>::iterator it = mydata.begin(); it != mydata.end(); ++it) {
		if (it->get_rva() == rva) {
			return &(*it);
		}
	}
	return NULL;
}

const Offset* Idatag_model::get_offset_byindex(int index) const 
{
	const Offset *offset = &mydata[index];
	return offset;
}

void Idatag_model::remove_offset(uint64_t rva)
{
	if (!check_rva(rva)) {
		return;
	}
	for (std::vector<Offset>::iterator it = mydata.begin(); it != mydata.end(); ++it) {
		if (it->get_rva() == rva) {
			it = mydata.erase(it);
			break;
		}
	}
}

void Idatag_model::add_feeder(std::string& feeder) 
{
	myfeeders.push_back(feeder);
}

void Idatag_model::remove_feeder(std::string& feeder) 
{
	for (std::vector<std::string>::iterator it = myfeeders.begin(); it != myfeeders.end(); ++it) {
		if (it->compare(feeder)) {
			it = myfeeders.erase(it);
			break;
		}
	}
}

void Idatag_model::gather_name_idb()
{
	for (int i = 0; i < get_nlist_size(); i++)
	{
		uint64_t ea = get_nlist_ea(i);
		const char* name = get_nlist_name(i);
		Offset offset = Offset(ea, name);
		add_offset(offset);
	}
}

void Idatag_model::gather_function_idb() 
{
	for (int i = 0; i < get_func_qty(); i++)
	{
		func_t* fn = getn_func(i);
		uint64_t func_ea = fn->start_ea;
		if (has_dummy_name(get_flags(func_ea))) 
		{
			qstring ida_name;
			get_ea_name(&ida_name, fn->start_ea);
			const char* offset_name = ida_name.c_str();
			Offset offset = Offset(fn->start_ea, offset_name);
			add_offset(offset);
		}
	}
}

void Idatag_model::import_info_disas() 
{
	gather_name_idb();
	gather_function_idb();
}

void Idatag_model::import_feed(json& j_feed, Offset& offset)
{
	uint64_t rva = j_feed["offset"];
	std::string label = j_feed["tag"];
	std::string feeder = j_feed["feeder"];

	if (!offset.check_already_tagged(label))
	{
		Tag tag = Tag(label, feeder);
		offset.add_tag(tag);
	}
}

void Idatag_model::import_feeds(json& j_feeds)
{
	try {
		for (auto it = j_feeds.begin(); it != j_feeds.end(); it++)
		{
			if (!((*it)["offset"].empty()
				&& (*it)["tag"].empty()
				&& (*it)["feeder"].empty()))
			{
				uint64_t rva = (*it)["offset"];
				Offset* offset = get_offset_byrva(rva);
				if (offset == NULL)
				{
					return;
				}
				import_feed(*it, *offset);
			}
		}
	}
	catch (json::exception& e) {
		msg("[IDATag] Error parsing file - %s\n", e.what());
	}
}

void Idatag_model::import_file(fs::path filepath)
{
	try {
		std::ifstream ifs(filepath);
		json j = json::parse(ifs);
		import_feeds(j);
	}
	catch(std::ifstream::failure e){
		msg("[IDATag] Error reading file - %s\n", e.what());
	}
}

void Idatag_model::import_files(fs::path& path_tags)
{
	try {
		for (const auto & entry : fs::directory_iterator(path_tags))
		{
			import_file(entry.path());
		}
	}
	catch(fs::filesystem_error& e)
	{
		msg("[IDATag] Error reading directory - %s\n", e.what());
	}
}

void Idatag_model::import_tags() 
{
	char curdir[QMAXPATH];
	try {
		qgetcwd(curdir, sizeof(curdir));
		fs::path current_path = fs::path(curdir);
		fs::path repository = fs::path("tags");
		fs::path tag_path = current_path /= repository;

		import_files(tag_path);
	}
	catch (fs::filesystem_error& e)
	{
		msg("[IDATag] Error reading directory - %s\n", e.what());
	}
}

void Idatag_model::export_tags()
{
	msg("[IDATAG] Export not yet implemented");
}

Offset::Offset(uint64_t rva_init, std::string name_init) 
{
	rva = rva_init;
	name = name_init;
}

Offset::Offset(uint64_t rva_init)
{
	rva = rva_init;
}

bool Offset::check_already_tagged(std::string label)
{
	for (std::vector<Tag>::iterator it = tags.begin(); it != tags.end(); ++it) {
		if (it->get_label().compare(label))
		{
			return true;
		}
	}
	return false;
}

void Offset::add_tag(Tag tag) 
{
	tags.push_back(tag);
}

void Offset::remove_tag(std::string label) 
{
	for (std::vector<Tag>::iterator it = tags.begin(); it != tags.end(); ++it) {
		if (it->get_label().compare(label)) {
			it = tags.erase(it);
			break;
		}
	}
}

uint64_t Offset::count_tags() 
{
	return tags.size();
}

uint64_t Offset::get_rva() const 
{
	return rva;
}

std::string Offset::get_name() const 
{
	return name;
}

Tag::Tag(std::string label_init, std::string signature_init)
{
	label = label_init;
	signature = signature_init;
}

Tag::Tag(std::string label_init, std::string type_init, std::string signature_init)
{
	label = label_init;
	type = type_init;
	signature = signature_init;
}

std::string Tag::get_label() const 
{
	return label;
}

std::string Tag::get_type() const 
{
	return type;
}

std::string Tag::get_signature() const 
{
	return signature;
}