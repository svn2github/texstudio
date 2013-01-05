#ifndef TEMPLATEMANAGER_P_H
#define TEMPLATEMANAGER_P_H

#include "mostQtHeaders.h"
#include "templatemanager.h"

// Abstract base class for templates
class Template {
public:
	~Template() { foreach (TemplateHandle *th, handles) { th->setTmpl(0); } }
	virtual QString name() const { return QString(); }
	virtual QString description() const { return QString(); }
	virtual QString author() const { return QString(); }
	virtual QString version() const { return QString(); }
	virtual QDate date() const { return QDate(); }
	virtual QString license() const { return QString(); }
	virtual QPixmap previewImage() const { return QPixmap(); }
	virtual QString file() const { return QString(); }
	virtual bool isEditable() const { return false; }

	void ref(TemplateHandle *th) {
		handles.append(th);
		//qDebug() << handles.size() << ">" << th->file() << this;
	}
	void deref(TemplateHandle *th) {
		//qDebug() << handles.size() << "<" << th->file() << this;
		handles.removeOne(th); }

	private:
	QList<TemplateHandle *> handles;
};


class LocalFileTemplate : public Template {
public:
	friend class LocalFileTemplateRessource;
	virtual QString name() const { return metaData["Name"]; }
	virtual QString description() const { return metaData["Description"]; }
	virtual QString author() const { return metaData["Author"]; }
	virtual QString version() const { return metaData["Version"]; }
	virtual QDate date() const;
	virtual QString license() const { return metaData["License"]; }
	virtual QPixmap previewImage() const { return QPixmap(imageFile()); }
	virtual QString file() const { return m_mainfile; }
	virtual bool isEditable() const { return m_editable; }

protected:
	LocalFileTemplate(QString mainfile);
	void init();
	virtual bool readMetaData() { return false; }
	virtual bool saveMetaData() { return false; }

	QHash<QString, QString> metaData;
private:
	QString imageFile() const;
	QString m_mainfile; // a single .tex file or a .zip for multiple files
	bool m_editable;
};


class LocalLatexTemplate : public LocalFileTemplate {
public:
	friend class LocalLatexTemplateRessource;
protected:
	virtual bool readMetaData();
	virtual bool saveMetaData();
private:
	LocalLatexTemplate(QString mainfile) : LocalFileTemplate(mainfile) { init(); }
};


class LocalTableTemplate : public LocalFileTemplate {
public:
	friend class LocalTableTemplateRessource;
protected:
	virtual bool readMetaData();
private:
	LocalTableTemplate(QString mainfile) : LocalFileTemplate(mainfile) { init(); }
};


class LocalFileTemplateRessource : public QObject, public AbstractTemplateRessource {
	Q_OBJECT
	Q_INTERFACES( AbstractTemplateRessource )
public:
	~LocalFileTemplateRessource();
	virtual QList<TemplateHandle> getTemplates();
	virtual bool isAccessible();
	virtual QString name() { return m_name; }
	virtual QString description() { return m_description; }
	void setDescription(const QString &descr) { m_description = descr; }
	virtual QIcon icon() { return m_icon; }
	void setEditable(bool b);
protected:
	LocalFileTemplateRessource(QString path, QStringList filters, QString name, QObject *parent = 0, QIcon icon=QIcon());
	virtual LocalFileTemplate* createTemplate(QString file) = 0;
	void update();
private:
	QString m_path;
	QStringList m_filters;
	QString m_name;
	QString m_description;
	QIcon m_icon;
	QList<LocalFileTemplate*> m_templates;
};

class LocalLatexTemplateRessource : public LocalFileTemplateRessource {
	Q_OBJECT
public:
	LocalLatexTemplateRessource(QString path, QString name, QObject *parent, QIcon icon)
		: LocalFileTemplateRessource(path, QStringList() << "*.tex", name, parent, icon) { update(); }
protected:
	virtual LocalFileTemplate* createTemplate(QString file) { return new LocalLatexTemplate(file); }
};

class LocalTableTemplateRessource : public LocalFileTemplateRessource {
	Q_OBJECT
public:
	LocalTableTemplateRessource(QString path, QString name, QObject *parent, QIcon icon)
		: LocalFileTemplateRessource(path, QStringList() << "*.js", name, parent, icon) { update(); }
protected:
	virtual LocalFileTemplate* createTemplate(QString file) { return new LocalTableTemplate(file); }
};


#endif // TEMPLATEMANAGER_P_H
