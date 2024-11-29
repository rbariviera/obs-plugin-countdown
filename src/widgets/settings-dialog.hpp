#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QWidget>
#include <QDialog>

#include <obs-frontend-api.h>
#include <obs-module.h>

#include "../plugin-support.h"
#include "../ui/ui_SettingsDialog.h"

class SettingsDialog : public QDialog {
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = nullptr,
				QString timerId = "");
	~SettingsDialog();

private:
	enum SourceType { TEXT_SOURCE = 1, SCENE_SOURCE = 2 };
	Ui::SettingsDialog *ui;

	void SetupDialogUI();
	void GetOBSSourceList();
	void ConnectUISignalHandlers();
	void ConnectObsSignalHandlers();

	static bool GetTextSources(void *list_property, obs_source_t *source);
	static void OBSSourceCreated(void *param, calldata_t *calldata);
	static void OBSSourceDeleted(void *param, calldata_t *calldata);
	static void OBSSourceRenamed(void *param, calldata_t *calldata);

	static int CheckSourceType(obs_source_t *source);

signals:

private slots:
	void EndMessageCheckBoxSelected(int state);
	void SceneSwitchCheckBoxSelected(int state);
	void ApplyButtonClicked();
	void CancelButtonClicked();
	void OkButtonClicked();
};

#endif // SETTINGSDIALOG_H
