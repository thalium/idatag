#include "model.hpp"

using json = nlohmann::json;


bool check_rva(uint64_t rva)
{
	return is_mapped(rva);
}

Idatag_model::Idatag_model(Idatag_configuration* configuration)
{
	this->myConfiguration = configuration;
}


void Idatag_model::init_model() 
{
	import_info_disas();
	import_tags();
}

const int Idatag_model::count_stats_tags() const
{
	int count = 0;
	for (const auto& it : this->mydata) {
		count += it.count_tags();
	}
	return count;
}

void Idatag_model::print_stats_model() 
{
	size_t nb_tags = count_stats_tags();
	size_t nb_offsets = this->mydata.size();
	msg("\n[IDATag] Tags : %d on %d offsets!\n", nb_tags, nb_offsets);
}

const std::vector<Offset>& Idatag_model::get_data() const 
{
	return this->mydata;
}

const std::vector<std::string>& Idatag_model::get_feeders() const 
{
	return this->myfeeders;
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
	return this->mydata.size();
}

int Idatag_model::columnCount(const QModelIndex &parent) const 
{
	return 3;
}

QVariant Idatag_model::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		switch (section) {
		case 0:
			return tr("Offset");
		case 1:
			return tr("Name");
		case 2:
			return tr("Tags");

		default:
			return QVariant();
		}
	}
	return QVariant();
}

QVariant Idatag_model::data(const QModelIndex &index, int role) const
{
	std::string name;
	QString qname;

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

	Offset* offset = get_offset_byindex(row);
	if (offset == NULL) return QVariant();

	switch(column) 
	{
		case 0:
			return offset->get_rva();
		case 1:
			name = offset->get_name();
			qname = QString::fromStdString(name);
			return qname;
		case 2:
			QVariant offset_variant = QVariant::fromValue(offset);
			return offset_variant;
	}
	return QVariant();
}

//bool Idatag_model::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {}
//bool Idatag_model::insertRows(int position, int rows, const QModelIndex $index = QModelIndex()) {}
//bool Idatag_model::removeRows(int position, int rows, const QModelIndex $index = QModelIndex()) {}

void Idatag_model::add_offset(const uint64_t& rva) 
{
	Offset offset(rva);
	this->mydata.push_back(offset);
}

void Idatag_model::add_offset(Offset& offset) 
{
	this->mydata.push_back(offset);
}

Offset* Idatag_model::get_offset_byrva(const uint64_t& rva)
{
	if (!check_rva(rva)) return NULL;
		
	for (auto& offset_it : this->mydata)
	{
		if (offset_it.get_rva() == rva)
		{
			return &offset_it;
		}
	}

	Offset offset = Offset(rva);
	add_offset(offset);

	for (auto& offset_it : this->mydata)
	{
		if (offset_it.get_rva() == rva)
		{
			return &offset_it;
		}
	}

	return NULL;
}

Offset* Idatag_model::get_offset_byindex(int index) const 
{
	const Offset* off = &this->mydata[index];
	Offset* offset = const_cast<Offset *>(off);
	return offset;
}

bool Idatag_model::compare_offset_rva(const uint64_t& rva, Offset& offset) const
{
	if (offset.get_rva() == rva) return true;
	return false;
}

QModelIndex Idatag_model::get_index_byrva(const uint64_t& rva)
{
	uint row = 0;
	for (const auto & offset : this->mydata)
	{
		row++;
		const uint64_t offset_rva = offset.get_rva();
		if (offset_rva == rva)
		{
			QModelIndex index = this->index(row, 0);
			return index;
		}
	}
	return QModelIndex();
}

/*
void Idatag_model::remove_offset(const uint64_t& rva) FIXME
{
	if (!check_rva(rva)) {
		return;
	}

	auto it = std::remove_if(mydata.begin(), mydata.end(), [&](Offset offset) {return compare_offset_rva(rva, offset); });
}
*/
void Idatag_model::add_feeder(std::string& feeder) 
{
	auto it = std::find(this->myfeeders.begin(), this->myfeeders.end(), feeder);
	if (it == this->myfeeders.end()) this->myfeeders.push_back(feeder);
}

void Idatag_model::remove_feeder(std::string& feeder) 
{
	auto it = std::find(this->myfeeders.begin(), this->myfeeders.end(), feeder);
	if (it != this->myfeeders.end()) this->myfeeders.erase(it);
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

void Idatag_model::import_feed(const json& j_feed, Offset& offset)
{
	uint64_t rva = j_feed["offset"];
	std::string label = j_feed["tag"];
	std::string feeder = j_feed["feeder"];

	if (!check_rva(rva)) return;
	if (offset.check_already_tagged(label)) return;

	Tag tag = Tag(label, feeder);
	offset.add_tag(tag);
	add_feeder(feeder);
}

void Idatag_model::import_feeds(json& j_feeds)
{
	try {
		for (const auto&  it : j_feeds)
		{
			if (!(it["offset"].empty() || it["tag"].empty() || it["feeder"].empty()))
			{
				uint64_t rva = it["offset"];
				Offset* offset = get_offset_byrva(rva);
				if (offset == NULL) return;
				
				import_feed(it, *offset);
			}
		}
	}
	catch (json::exception& e) {
		msg("\n[IDATag] Error parsing file - %s", e.what());
	}
}

void Idatag_model::import_file(const fs::path& filepath)
{
	try {
		std::ifstream ifs(filepath);
		json j = json::parse(ifs);
		import_feeds(j);
	}
	catch(std::ifstream::failure e){
		msg("\n[IDATag] Error reading file - %s", e.what());
	}
}

void Idatag_model::import_files(const fs::path& path_tags)
{
	try {
		for (const auto & entry : fs::directory_iterator(path_tags))
		{
			import_file(entry.path());
		}
	}
	catch(fs::filesystem_error& e)
	{
		msg("\n[IDATag] Error reading directory - %s", e.what());
	}
}

void Idatag_model::import_tags() 
{
	char curdir[QMAXPATH];
	char filename[QMAXPATH];
	try {
		qgetcwd(curdir, sizeof(curdir));
		fs::path current_path = fs::path(curdir);
		fs::path repository = fs::path("tags");
		fs::path tag_path = current_path /= repository;
		get_root_filename(filename, QMAXPATH - 1);
		fs::path spec_tag_path = tag_path /= filename;

		import_files(spec_tag_path);
	}
	catch (fs::filesystem_error& e)
	{
		msg("\n[IDATag] Error reading directory - %s", e.what());
	}
}

void Idatag_model::export_tags() const
{
	try {
		json jsonArray = json::array();
		std::ofstream jsonFile;
		fs::path file;
		for (const auto & offset : mydata)
		{
			std::vector<Tag> tags = offset.get_tags();
			for (const auto & tag : tags)
			{
				json jsonTag;
				
				jsonTag["offset"] = offset.get_rva();
				jsonTag["name"] = offset.get_name();
				jsonTag["tag"] = tag.get_label();
				jsonTag["feeder"] = tag.get_signature();
				jsonTag["type"] = tag.get_type();

				jsonArray.push_back(jsonTag);
			}
		}

		char curdir[QMAXPATH];
		char filename[QMAXPATH];

		if (this->myConfiguration->get_path_configuration().empty())
		{
			qgetcwd(curdir, sizeof(curdir));
			fs::path current_path = fs::path(curdir);
			fs::path repository = fs::path("tags");
			fs::path tag_path = current_path /= repository;
			get_root_filename(filename, QMAXPATH - 1);
			fs::path spec_tag_path = tag_path /= filename;
			spec_tag_path /= filename;
			fs::path extension = fs::path(".json");
			file = spec_tag_path += extension;
		}
		else {
			fs::path tag_path = fs::path(this->myConfiguration->get_path_configuration());
			get_root_filename(filename, QMAXPATH - 1);
			fs::path spec_tag_path = tag_path /= filename;
			spec_tag_path /= filename;
			fs::path extension = fs::path(".json");
			file = spec_tag_path += extension;
		}
		jsonFile.open(file);
		jsonFile << jsonArray.dump().c_str();
		jsonFile.close();
		msg("\n[IDATag] Tags exported !");
	}
	catch (fs::filesystem_error& e)
	{
		msg("\n[IDATag] Error exporting tags - %s", e.what());
	}
}

Offset::Offset()
{
	this->rva = 0;
	this->name = "";
}

Offset::Offset(const Offset& offset)
{
	this->rva = offset.get_rva();
	this->name = offset.get_name();
	this->tags = offset.get_tags();
}

Offset::Offset(const uint64_t& rva, const std::string& name)
{
	this->rva = rva;
	this->name = name;
}

Offset::Offset(const uint64_t& rva)
{
	this->rva = rva;
}

bool Offset::check_already_tagged(std::string& label) const
{
	for (const auto & tag : this->tags)
	{
		const std::string lbl = tag.get_label();
		if (lbl.compare(label) == 0) return true;
	}
	return false;
}

void Offset::add_tag(Tag& tag) 
{
	std::string lbl = tag.get_label();
	if(!check_already_tagged(lbl)) this->tags.push_back(tag);
}

const std::vector<Tag> Offset::get_tags() const
{
	return this->tags;
}

bool Offset::compare_tag_label(const std::string& label, Tag& tag) const
{
	if (tag.get_label().compare(label) == 0) {
		return true;
	}
	return false;
}

void Offset::remove_tag(std::string& label) 
{
	auto it = std::remove_if(this->tags.begin(), this->tags.end(), [&](Tag tag) {return compare_tag_label(label, tag); });
	this->tags.erase(it);
}

const uint64_t Offset::count_tags() const
{
	return this->tags.size();
}

const uint64_t Offset::get_rva() const 
{
	return this->rva;
}

const std::string Offset::get_name() const 
{
	return this->name;
}

void Offset::set_name(std::string name)
{
	this->name = name;
}

Tag::Tag(std::string& label, std::string& signature)
{
	this->label = label;
	this->signature = signature;
}

Tag::Tag()
{
	this->label = "";
	this->type = "";
	this->signature = "";
}

Tag::Tag(std::string& label, std::string& type, std::string& signature)
{
	this->label = label;
	this->type = type;
	this->signature = signature;
}

const std::string Tag::get_label() const 
{
	return this->label;
}

const std::string Tag::get_type() const 
{
	return this->type;
}

const std::string Tag::get_signature() const 
{
	return this->signature;
}