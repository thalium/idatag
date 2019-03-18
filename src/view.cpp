#include "view.hpp"

Idatag_view::Idatag_view(QT::QWidget* parent_init, Idatag_model* myModel_init) {

	myModel = myModel_init;
	parent = parent_init;

	tb = new QTableView();
	tb->setSortingEnabled(true);
	tb->setModel(myModel);

	tb->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	tb->resizeColumnsToContents();

	hheader = tb->horizontalHeader();
	hheader->setSectionsMovable(true);
	hheader->setSectionResizeMode(0, QHeaderView::Interactive);
	hheader->setSectionResizeMode(1, QHeaderView::Interactive);
	hheader->setSectionResizeMode(2, QHeaderView::Stretch);
	hheader->setDefaultAlignment(Qt::AlignLeft);

	cbox = new QCheckBox();
	cbox->setText("Filter not tagged offsets");

	tf = new QLineEdit();
	QLabel* tfl = new QLabel();
	tfl->setText("Search: ");

	layout = new QGridLayout(parent);
	layout->addWidget(tb, 0, 0, 1, 0);
	layout->addWidget(tfl, 1, 0);
	layout->addWidget(tf, 1, 1);
	layout->addWidget(cbox, 1, 2);
}

Idatag_view::~Idatag_view() {}