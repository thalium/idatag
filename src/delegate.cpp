#include "delegate.hpp"


Idatag_delegate_rva::Idatag_delegate_rva(QWidget* parent, Idatag_model* myModel, Idatag_configuration* myConfiguration) :
	QStyledItemDelegate()
{
	this->parent = parent;
	this->myModel = myModel;
	this->myConfiguration = myConfiguration;
}

void Idatag_delegate_rva::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
	char rva_str[20];
	if (!index.isValid()) return;

	if (index.data().canConvert<uint64_t>())
	{
		uint64_t rva = qvariant_cast<uint64_t>(index.data());
		if (this->myConfiguration->get_address_width_configuration() == 16)
		{
			snprintf(rva_str, 19, "0x%016llX", (unsigned long long)rva);
		}
		else
		{
			snprintf(rva_str, 19, "0x%08llX", (unsigned long long)rva);
		}

		painter->save();

		if (option.state & QStyle::State_Selected)
		{
			painter->fillRect(option.rect, option.palette.highlight());
			painter->setPen(Qt::white);
		}

		QStyleOptionViewItem option_b = option;
		option_b.rect.setX(option.rect.x() + 5);
		painter->drawText(option_b.rect, Qt::AlignVCenter | Qt::AlignLeft, rva_str);
		painter->restore();
	}
	else {
		QStyledItemDelegate::paint(painter, option, index);
	}
}

Idatag_delegate_name::Idatag_delegate_name(QWidget* parent, Idatag_model* myModel, Idatag_configuration* myConfiguration) :
	QStyledItemDelegate()
{
	this->parent = parent;
	this->myModel = myModel;
	this->myConfiguration = myConfiguration;
}

void Idatag_delegate_name::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
	if (!index.isValid()) return;

	if (index.data().canConvert<QString>())
	{
		QString name = qvariant_cast<QString>(index.data());
		painter->save();
		if (option.state & QStyle::State_Selected)
		{
			painter->fillRect(option.rect, option.palette.highlight());
			painter->setPen(Qt::white);
		}
		QStyleOptionViewItem option_b = option;
		option_b.rect.setX(option.rect.x() + 5);
		painter->drawText(option_b.rect, Qt::AlignVCenter | Qt::AlignLeft, name);
		painter->restore();
	} else {
		QStyledItemDelegate::paint(painter, option, index);
	}
}

Idatag_delegate_tag::Idatag_delegate_tag(QWidget* parent, Idatag_model* myModel, Idatag_configuration* myConfiguration, Idatag_proxy* myProxy) :
	QStyledItemDelegate()
{
	this->parent = parent;
	this->myModel = myModel;
	this->myPalette = new Idatag_palette(this->myModel->get_feeders());
	this->myConfiguration = myConfiguration;
	this->myProxy = myProxy;
}

Idatag_editor::Idatag_editor(QWidget* parent)
	: QLineEdit(parent)
{
	this->lock = true;
	connect(this, &QLineEdit::selectionChanged, this, &Idatag_editor::OnEdit);
}

void Idatag_editor::locker()
{
	this->lock = true;
}

void Idatag_editor::OnEdit()
{
	if (this->lock) {
		int size = this->text().size();
		this->setSelection(size, 0);
	}
	this->lock = false;
}

QWidget* Idatag_delegate_tag::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Idatag_editor* editor = new Idatag_editor(parent);
	return editor;
}

void Idatag_delegate_tag::drawTag(Tag tag, QPainter *painter, const QStyleOptionViewItem& option, int width, int height, bool filtered) const
{
	painter->setPen(Qt::white);
	QString qlabel = QString::fromStdString(tag.get_label());
	QPainterPath path = QPainterPath();
	path.addRoundedRect(QRectF(option.rect.x(), option.rect.y() + 2, width + 20, height), 10, 10);
	painter->fillPath(path, this->myPalette->get_feeder_colour(tag.get_signature()));

	painter->drawText(path.controlPointRect(), Qt::AlignHCenter | Qt::AlignVCenter, qlabel);
	painter->setPen(Qt::NoPen);
	if (filtered)
	{
		QPen pen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		painter->setPen(pen);
	}
	painter->drawPath(path);
}

void Idatag_delegate_tag::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (!index.isValid()) return;

	this->myPalette->refresh_feeders(this->myModel->get_feeders());

	Offset* offset = index.data().value<Offset*>();
	if (offset != NULL) {
		
		std::vector<Tag> tags= offset->get_tags();

		painter->save();

		if (option.state & QStyle::State_Selected)
		{
			painter->fillRect(option.rect, option.palette.highlight());
		}

		painter->setRenderHint(QPainter::Antialiasing);
		painter->translate(10, 0);
		int trans = 30;
		bool filtered;
		for (auto &tag : tags) {
			QString qlabel = QString::fromStdString(tag.get_label());
			QFontMetrics fontmetrics = painter->fontMetrics();
			int width = fontmetrics.boundingRect(qlabel).width() + 7;
			int height = fontmetrics.boundingRect(qlabel).height() + 7;

			filtered = this->myProxy->is_label_filtered(tag.get_label());

			drawTag(tag, painter, option, width, height, filtered);

			painter->translate(width + trans, 0);
		}
		painter->restore();
	}
	else {
		QStyledItemDelegate::paint(painter, option, index);
	}
}

void Idatag_delegate_tag::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	if (!index.isValid()) return;

	Offset* offset = index.data().value<Offset*>();
	if (offset != NULL) {

		Idatag_editor* qeditor = qobject_cast<Idatag_editor*>(editor);
		qeditor->locker();
		QString qinput = qeditor->text();
		std::string input = qinput.toStdString();
		if (input.find_first_not_of(" ") != std::string::npos)
		{
			QStringList labels = qeditor->text().split(" ", QString::SkipEmptyParts);
			std::vector<Tag> tags = offset->get_tags();
			std::string user = myConfiguration->get_username_configuration();
			std::string feeder = user;
			bool changed = false;

			for (auto& tag : tags)
			{
				bool still = false;
				for (const auto & qlabel : labels)
				{
					std::string label = qlabel.toStdString();
					if (tag.get_label().compare(label) == 0)
					{
						still = true;
						break;
					}
				}
				if (still == false)
				{
					std::string lbl_toremove = tag.get_label();
					offset->remove_tag(lbl_toremove);
					changed = true;
				}
			}
			for (const auto & qlabel : labels)
			{
				std::string label = qlabel.toStdString();
				if (!offset->check_already_tagged(label)) {

					Tag tag = Tag(label, feeder);
					offset->add_tag(tag);

					std::string autofeed = "IDATag";
					Tag tag_user = Tag(user, autofeed);
					offset->add_tag(tag_user);

					this->myModel->add_feeder(feeder);
					this->myModel->add_feeder(autofeed);
					changed = true;
				}
			}
			if (changed) {
				std::string autofeed = "IDATag";
				Tag tag_user = Tag(user, autofeed);
				offset->add_tag(tag_user);
				this->myModel->add_feeder(autofeed);
			}
		}
		else
		{
			offset->remove_all_tags();
		}
	}
	else {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

void Idatag_delegate_tag::setEditorData(QWidget *editor, const QModelIndex &index) const 
{
	if (!index.isValid()) return;

	Offset* offset = index.data().value<Offset*>();
	if (offset != NULL) {
		std::vector<Tag> tags = offset->get_tags();

		std::string tag_labels;
		for (auto &tag : tags) {
			tag_labels += tag.get_label();
			tag_labels += " ";
		}
		QString qtag_labels = QString::fromStdString(tag_labels);

		QLineEdit* qeditor = qobject_cast<QLineEdit*>(editor);
		if (qeditor) {   
			qeditor->setText(qtag_labels);
		}
	}
	else {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}