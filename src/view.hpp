#include <QtWidgets/QTableView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMenu>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtCore/Qt>

#include "model.hpp"
#include "proxy.hpp"

class Idatag_view : public QT::QWidget{
	
private:
	Idatag_model*		myModel;
	Idatag_proxy*		myProxy;
	QWidget*			parent;
	QTableView*			tb;
	QHeaderView*		hheader;
	QCheckBox*			cbox;
	QLineEdit*			tf;
	QLabel*				tfl;
	QKeySequence*		sc_filter_sequence;
	QShortcut*			sc_filter;
	QGridLayout*		layout;

public:
	Idatag_view(QT::QWidget*, Idatag_model*);
	~Idatag_view();

	void OnFilter_empty();
	void OnFilter_string();
};
