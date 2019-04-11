#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtCore/qobject.h>

#include <ida.hpp>
#include <kernwin.hpp>
#include <name.hpp>
#include <funcs.hpp>
#include <bytes.hpp>

#include "json.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>

using json = nlohmann::json;
namespace fs = std::filesystem;

class Tag {
private:
	std::string					label;
	std::string					type;
	std::string					signature;
public:
	Tag();
	Tag(std::string&, std::string&);
	Tag(std::string&, std::string&, std::string&);
	const std::string get_label() const;
	const std::string get_type() const;
	const std::string get_signature() const;
};

class Offset : public QObject{
	Q_OBJECT
private:
	uint64_t					rva;
	std::string					name;
	std::vector<Tag>			tags;
public:
	Offset();
	Offset(const Offset&);
	Offset(const uint64_t&);
	Offset(const uint64_t&, const std::string&);

	const uint64_t get_rva() const ;
	const std::string get_name() const;

	bool check_already_tagged(std::string&) const;
	bool compare_tag_label(const std::string&, Tag&) const;

	void add_tag(Tag&);
	const std::vector<Tag> get_tags() const;
	void remove_tag(std::string&);
	const uint64_t count_tags() const;
};
Q_DECLARE_METATYPE(Offset*)
Q_DECLARE_OPAQUE_POINTER(Offset*)

class Idatag_model : public QT::QAbstractTableModel {
	Q_OBJECT
private:
	std::vector<Offset>			mydata;
	std::vector<std::string>	myfeeders;

public:
	void init_model();
	const int count_stats_tags() const;
	void print_stats_model();

	const std::vector<Offset>& get_data() const;
	
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	//bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	//bool insertRows(int position, int rows, const QModelIndex $index = QModelIndex());
	//bool removeRows(int position, int rows, const QModelIndex $index = QModelIndex());
	
	void add_offset(const uint64_t&);
	void add_offset(Offset&);
	//void remove_offset(const uint64_t&);
	Offset* get_offset_byrva(const uint64_t&);
	Offset* get_offset_byindex(int) const ;
	bool compare_offset_rva(const uint64_t&, Offset&) const;

	void add_feeder(std::string&);
	void remove_feeder(std::string&);
	const std::vector<std::string>& get_feeders() const;

	void gather_name_idb();
	void gather_function_idb();
	void import_info_disas();
	
	void import_feed(const json&, Offset&);
	void import_feeds(json&);
	void import_file(const fs::path&);
	void import_files(const fs::path&);
	void import_tags();
	void export_tags() const;
};