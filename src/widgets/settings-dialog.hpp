#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QPushButton>

#include <obs-frontend-api.h>
#include <obs-module.h>

#include "../plugin-support.h"
#include "../utils/timer-utils.hpp"
#include "../ui/ui_SettingsDialog.h"

class SettingsDialog : public QDialog {
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = nullptr,
				TimerWidgetStruct *tData = nullptr);
	~SettingsDialog();

	void SetCountUpCheckBoxEnabled(bool isEnabled);

private:
	enum SourceType { TEXT_SOURCE = 1, SCENE_SOURCE = 2 };
	Ui::SettingsDialog *ui;
	TimerWidgetStruct *timerData;

	void SetupDialogUI(TimerWidgetStruct *settingsDialogData);
	void GetOBSSourceList();
	void ConnectUISignalHandlers();
	void ConnectObsSignalHandlers();
	void ApplyFormChanges();
	void SetFormDetails(TimerWidgetStruct *settingsDialogData);

	static bool GetTextSources(void *list_property, obs_source_t *source);
	static void OBSSourceCreated(void *param, calldata_t *calldata);
	static void OBSSourceDeleted(void *param, calldata_t *calldata);
	static void OBSSourceRenamed(void *param, calldata_t *calldata);

	static int CheckSourceType(obs_source_t *source);

signals:
	void SettingsUpdated();

private slots:
	void FormChangeDetected();
	void EndMessageCheckBoxSelected(int state);
	void SceneSwitchCheckBoxSelected(int state);
	void ApplyButtonClicked();
	void CancelButtonClicked();
	void OkButtonClicked();
};

#endif // SETTINGSDIALOG_H
