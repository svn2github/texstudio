#include "leftpanel.h"

Q_DECLARE_METATYPE(QAction*)


LeftPanel::LeftPanel(QWidget *parent) :
	AutoCollapsingPanel(parent), vLayout(0), topbar(0), collapsedTopbar(0), contentsArea(0), toolbox(0), stack(0), sidebar(0), cbTopbarSelector(0)
{
	topbar = new QToolBar("LeftPanelTopBar", this);
	topbar->setOrientation(Qt::Horizontal);
	topbar->setFloatable(false);
	topbar->setMovable(false);
	topbar->setIconSize(QSize(16,16));
	topbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	// TODO: the combobox in the the top bar prevents shrinking to 23px. Didn't manage to
	// get it shrunken properly (is at least 37px), even when hiding (use QAction::setVisible
	// see documentation of QToolBar::addWidget)
	// Workaround: exchange the topbar when collapsing
	collapsedTopbar = new QToolBar(this);
	collapsedTopbar->setOrientation(Qt::Horizontal);
	collapsedTopbar->setFloatable(false);
	collapsedTopbar->setMovable(false);
	collapsedTopbar->setIconSize(QSize(16,16));
	collapsedTopbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	collapsedTopbar->setVisible(false);

	QAction *act = new QAction(this);
	act->setIcon(QIcon(":/images/test/arrowleft.png"));
	act->setToolTip(tr("Auto Collapse"));
	act->setCheckable(true);
	topbar->addAction(act);
	collapsedTopbar->addAction(act);
	connect(act, SIGNAL(toggled(bool)), this, SLOT(setCollapsingEnabled(bool)));

	cbTopbarSelector = new QComboBox();
	cbTopbarSelector->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	cbTopbarSelector->setMinimumWidth(0);
	cbTopbarSelector->setMaxVisibleItems(25);
	topbar->addWidget(cbTopbarSelector);
	connect(cbTopbarSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(showPageFromComboBox(int)));
	connect(this, SIGNAL(collapseStateChanged(CollapseState)), SLOT(updateToolbarForResizing(CollapseState)));

	act = new QAction(this);
	act->setIcon(QIcon(":/images/test/Transition.png"));
	act->setToolTip(tr("Animated Motion: Temporary button for toggling animation\nMove to config or context menu of collapse button"));
	act->setCheckable(true);
	topbar->addAction(act);
	connect(act, SIGNAL(toggled(bool)), this, SLOT(setAnimatedMotion(bool)));

	act = new QAction(this);
	act->setIcon(QIcon(":/images/test/closebutton.png"));
	act->setToolTip(tr("Close"));
	topbar->addAction(act);
	connect(act, SIGNAL(clicked()), this, SLOT(hide()));

	contentsArea = new QWidget();
	contentsArea->setContentsMargins(0,0,0,0);

	vLayout = new QVBoxLayout(this);
	vLayout->setSpacing(0);
	vLayout->setMargin(0);
	vLayout->addWidget(topbar);
	vLayout->addWidget(contentsArea);
	setLayout(vLayout);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this,SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));

}

void LeftPanel::addWidget(QWidget* widget, const QString& id, const QString& text, const QString& iconName){
	widgets << widget;
	widget->setProperty("id",id);
	widget->setProperty("Name",text);
	widget->setProperty("iconName",iconName);
	widget->setProperty("StructPos",widgets.size());

	QAction *Act = new QAction(text, this);
	Act->setCheckable(true);
	Act->setChecked(!hiddenWidgetsIds.contains(id));
	Act->setData(id);
	connect(Act, SIGNAL(toggled(bool)), this, SLOT(toggleWidgetVisibleFromAction(bool)));
	addAction(Act);
}
void LeftPanel::setWidgetText(const QString& id, const QString& text){
	setWidgetText(widget(id),text);
}
void LeftPanel::setWidgetText(QWidget* widget, const QString& text){
	int pos=widgets.indexOf(widget);
	if (pos<0) return;
	widget->setProperty("Name",text);
	if (newStyle) actions()[pos]->setToolTip(text);
	else toolbox->setItemText(pos,text);
}
void LeftPanel::setWidgetIcon(const QString& id, const QString& icon){
	setWidgetIcon(widget(id), icon);
}

void LeftPanel::setWidgetIcon(QWidget* widget, const QString& icon){
	int pos=widgets.indexOf(widget);
	if (pos<0) return;
	widget->setProperty("iconName",icon);
}

void LeftPanel::showPageFromComboBox(int index){
	showPage(cbTopbarSelector->itemData(index).toString());
}

void LeftPanel::showPageFromAction(){
	QAction* act=qobject_cast<QAction*>(sender());
	if (!act) return;
	showPage(act->data().toString());
}

void LeftPanel::showPage(const QString &id){
	QWidget* wid=widget(id);
	stack->setCurrentWidget(wid);
	foreach (QAction* a, sidebar->actions())
		a->setChecked(a->data().toString()==id);
	int index = cbTopbarSelector->findData(id);
	if (index >= 0)
		cbTopbarSelector->setCurrentIndex(index);
}

void LeftPanel::currentWidgetChanged(int i){
	Q_ASSERT(newStyle==false);
	setWindowTitle(toolbox->itemText(i));
}
void LeftPanel::toggleWidgetVisibleFromAction(bool on){
	QAction* act=qobject_cast<QAction*>(sender());
	if (!act || act->data().toString()=="") return;
	if (on)
		hiddenWidgetsIds.removeAll(act->data().toString());
	else if (!hiddenWidgetsIds.contains(act->data().toString()))
		hiddenWidgetsIds.append(act->data().toString());
	showWidgets(newStyle);
}

void LeftPanel::updateToolbarForResizing(CollapseState clState) {
	if (clState == Collapsing) {
		vLayout->removeWidget(topbar);
		topbar->hide();
		vLayout->insertWidget(0, collapsedTopbar);
		collapsedTopbar->show();
	} else if (clState == Expanded) {
		vLayout->removeWidget(collapsedTopbar);
		collapsedTopbar->hide();
		vLayout->insertWidget(0, topbar);
		topbar->show();
	}
}

void LeftPanel::customContextMenuRequested(const QPoint& localPosition){
	QWidget *widget=currentWidget();
	if(widget && widget->underMouse()) //todo?: use a more reliable function than underMouse (see qt bug 260000)
		emit widgetContextMenuRequested(widget, mapToGlobal(localPosition));
	else{
		QMenu menu;
		menu.addActions(actions());
		menu.exec(mapToGlobal(localPosition));
	}
}

void LeftPanel::showWidgets(bool newLayoutStyle){
	if (toolbox) {
		for (int i=0;i<widgets.count();i++){
			toolbox->removeItem(toolbox->indexOf(widgets[i]));
			widgets[i]->setParent(this);//otherwise it will be deleted
		}
		delete toolbox;
	}
	if (stack) {
		for (int i=0;i<widgets.count();i++){
			stack->removeWidget(widgets[i]);
			widgets[i]->setParent(this);//otherwise it will be deleted
		}
		delete stack;
	}
	if (sidebar) delete sidebar;
	newStyle=newLayoutStyle;
	if (newLayoutStyle) {
		toolbox=0;
		setLineWidth(0);
		setFrameShape(QFrame::Box);
		setFrameShadow(QFrame::Plain);

		sidebar=new QToolBar("LogToolBar",this);
		sidebar->setFloatable(false);
		sidebar->setOrientation(Qt::Vertical);
		sidebar->setMovable(false);
		sidebar->setIconSize(QSize(16,16));

		cbTopbarSelector->clear();

		stack=new QStackedWidget(this);

		for (int i=0;i<widgets.size();i++)
			if (!hiddenWidgetsIds.contains(widgetId(widgets[i]))) {
				stack->addWidget(widgets[i]);
				widgets[i]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
				widgets[i]->setMinimumWidth(0);
				QAction* act=sidebar->addAction(QIcon(widgets[i]->property("iconName").toString()),widgets[i]->property("Name").toString());
				cbTopbarSelector->addItem(widgets[i]->property("Name").toString(), widgets[i]->property("id"));
				act->setCheckable(true);
				if (i==0) act->setChecked(true);
				act->setData(widgetId(widgets[i]));
				connect(act,SIGNAL(triggered()),this,SLOT(showPageFromAction()));
				widgets[i]->setProperty("associatedAction", QVariant::fromValue<QAction*>(act));
			} else widgets[i]->hide();

		QHBoxLayout* hlayout= new QHBoxLayout(contentsArea);
		hlayout->setSpacing(0);
		hlayout->setMargin(0);
		hlayout->addWidget(sidebar);
		hlayout->addWidget(stack);
	} else {
		sidebar=0;
		stack=0;
		toolbox = new QToolBox(contentsArea);
		for (int i=0;i<widgets.size();i++)
			if (!hiddenWidgetsIds.contains(widgetId(widgets[i]))) {
				toolbox->addItem(widgets[i],QIcon(widgets[i]->property("iconName").toString()),widgets[i]->property("Name").toString());
			} else widgets[i]->hide();
		connect(toolbox,SIGNAL(currentChanged(int)),SLOT(currentWidgetChanged(int)));
//TODO
		//setWidget(toolbox);

	}
	if (!widgets.empty()) //name after active (first) widget
		setWindowTitle(widgets.first()->property("Name").toString());
}

int LeftPanel::widgetCount() const{
	return widgets.count();
}
void LeftPanel::setHiddenWidgets(const QString& hidden){
	hiddenWidgetsIds=hidden.split("|");
}
QString LeftPanel::hiddenWidgets() const{
	return hiddenWidgetsIds.join("|");
}

/*'void CustomWidgetList::addWidgetOld(QWidget* widget, const QString& text, const QIcon& icon){
}
void CustomWidgetList::addWidgetNew(QWidget* widget, const QString& text, const QIcon& icon){
	stack->addWidget(*list);
	toolbar->addAction(icon,text);
}*/

QWidget* LeftPanel::widget(int i) const{
	return widgets[i];
}
QWidget* LeftPanel::widget(const QString& id) const{
	for (int i=0;i<widgets.size();i++)
		if (widgetId(widgets[i])==id)
			return widgets[i];
	return 0;
}
void LeftPanel::setCurrentWidget(QWidget* widget){
	if (newStyle) {
		stack->setCurrentWidget(widget);
		QAction* act = widget->property("associatedAction").value<QAction*>();
		foreach (QAction* a, sidebar->actions())
			a->setChecked(a==act);
	} else {
		toolbox->setCurrentWidget(widget);
	}
}
QWidget* LeftPanel::currentWidget() const{
	if (newStyle) return stack->currentWidget();
	else return toolbox->currentWidget();
}
bool LeftPanel::isNewLayoutStyleEnabled() const{
	return newStyle;
}
QString LeftPanel::widgetId(QWidget* widget) const{
	if (!widget) return "";
	return widget->property("id").toString();
}


