#pragma once

#include <QtCore/qabstractitemmodel.h>

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

using json = nlohmann::json;
namespace fs = std::filesystem;

class Tag {
private:
	std::string					label;
	std::string					type;
	std::string					signature;
public:
	Tag(std::string, std::string);
	Tag(std::string, std::string, std::string);
	std::string get_label() const;
	std::string get_type() const;
	std::string get_signature() const;
};

class Offset {
private:
	uint64_t					rva;
	std::string					name;
	std::vector<Tag>			tags;
public:
	Offset(uint64_t);
	Offset(uint64_t, std::string);

	uint64_t get_rva() const ;
	std::string get_name() const;

	bool check_already_tagged(std::string);

	void add_tag(Tag);
	void remove_tag(std::string);
	uint64_t count_tags();
};

class Idatag_model : public QT::QAbstractTableModel {
	
private:
	std::vector<Offset>			mydata;
	std::vector<std::string>	myfeeders;

public:
	void init_model();
	int count_stats_tags();
	void print_stats_model();

	std::vector<Offset> get_data() const;
	
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	//bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	//bool insertRows(int position, int rows, const QModelIndex $index = QModelIndex());
	//bool removeRows(int position, int rows, const QModelIndex $index = QModelIndex());
	
	void add_offset(uint64_t);
	void add_offset(Offset&);
	void remove_offset(uint64_t);
	Offset* get_offset_byrva(uint64_t);
	const Offset* get_offset_byindex(int) const ;

	void add_feeder(std::string&);
	void remove_feeder(std::string&);
	std::vector<std::string> get_feeders() const;

	void gather_name_idb();
	void gather_function_idb();
	void import_info_disas();
	
	void import_feed(json&, Offset&);
	void import_feeds(json&);
	void import_file(fs::path);
	void import_files(fs::path&);
	void import_tags();
	void export_tags();
};