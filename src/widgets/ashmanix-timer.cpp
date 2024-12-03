#include "ashmanix-timer.hpp"

AshmanixTimer::AshmanixTimer(QWidget *parent, QString id,
			     obs_websocket_vendor vendor)
	: QWidget(parent),
	  ui(new Ui::AshmanixTimer)
{
	countdownTimerData = TimerWidgetStruct();

	ui->setupUi(this);
	countdownTimerData.timerId = id;
	this->setProperty("id", countdownTimerData.timerId);
	vendor = &vendor;

#if __APPLE__
	// ui->hsDaysLeft->changeSize(0, 20, QSizePolicy::Fixed,
	// 			   QSizePolicy::Fixed);
	// ui->hsDaysRight->changeSize(0, 20, QSizePolicy::Fixed,
	// 			    QSizePolicy::Fixed);
	// ui->hsHoursLeft->changeSize(0, 20, QSizePolicy::Fixed,
	// 			    QSizePolicy::Fixed);
	// ui->hsHoursRight->changeSize(0, 20, QSizePolicy::Fixed,
	// 			     QSizePolicy::Fixed);
	// ui->hsMinutesLeft->changeSize(0, 20, QSizePolicy::Fixed,
	// 			      QSizePolicy::Fixed);
	// ui->hsMinutesRight->changeSize(0, 20, QSizePolicy::Fixed,
	// 			       QSizePolicy::Fixed);
	// ui->hsSecondsLeft->changeSize(0, 20, QSizePolicy::Fixed,
	// 			      QSizePolicy::Fixed);
	// ui->hsSecondsRight->changeSize(0, 20, QSizePolicy::MinimumExpanding,
	// 			       QSizePolicy::MinimumExpanding);
#endif

	SetupTimerWidgetUI();

	ConnectUISignalHandlers();

	InitialiseTimerTime();
}

AshmanixTimer::~AshmanixTimer()
{
	this->deleteLater();
}

void AshmanixTimer::SaveTimerWidgetDataToOBSSaveData(
	obs_data_t *dataObject)
{
	obs_data_set_string(dataObject, "timerId",
			    countdownTimerData.timerId.toStdString().c_str());
	obs_data_set_bool(dataObject, "shouldCountUp",
			  countdownTimerData.shouldCountUp);
	obs_data_set_bool(dataObject, "showLeadingZero",
			  countdownTimerData.showLeadingZero);

	obs_data_set_string(dataObject, "selectedSource",
			    countdownTimerData.selectedSource.toStdString().c_str());
	obs_data_set_string(dataObject, "selectedScene",
			    countdownTimerData.selectedScene.toStdString().c_str());
	obs_data_set_string(dataObject, "endMessage",
			    countdownTimerData.endMessage.toStdString().c_str());
	obs_data_set_string(
		dataObject, "dateTime",
		countdownTimerData.dateTime.toString().toStdString().c_str());

	obs_data_set_int(dataObject, "periodDays", countdownTimerData.periodDays);
	obs_data_set_int(dataObject, "periodHours", countdownTimerData.periodHours);
	obs_data_set_int(dataObject, "periodMinutes", countdownTimerData.periodMinutes);
	obs_data_set_int(dataObject, "periodSeconds", countdownTimerData.periodSeconds);

	obs_data_set_bool(dataObject, "showDays", countdownTimerData.showDays);
	obs_data_set_bool(dataObject, "showHours", countdownTimerData.showHours);
	obs_data_set_bool(dataObject, "showMinutes", countdownTimerData.showMinutes);
	obs_data_set_bool(dataObject, "showSeconds", countdownTimerData.showSeconds);
	obs_data_set_bool(dataObject, "showEndMessage",
			  countdownTimerData.showEndMessage);
	obs_data_set_bool(dataObject, "showEndScene", countdownTimerData.showEndScene);

	obs_data_set_int(dataObject, "countdownTypeSelectedTab",
			 countdownTimerData.countdownTypeSelectedTab);

	// ------------------------- Hotkeys -------------------------
	SaveHotkey(dataObject, countdownTimerData.startCountdownToTimeHotkeyId,
		   ("Ashmanix_Countdown_Timer_Start_" +
		    (countdownTimerData.timerId).toStdString())
			   .c_str());

	SaveHotkey(dataObject, countdownTimerData.startCountdownToTimeHotkeyId,
		   ("Ashmanix_Countdown_Timer_Set_" +
		    (countdownTimerData.timerId).toStdString())
			   .c_str());

	SaveHotkey(dataObject, countdownTimerData.startCountdownToTimeHotkeyId,
		   ("Ashmanix_Countdown_Timer_To_Time_Start_" +
		    (countdownTimerData.timerId).toStdString())
			   .c_str());

	SaveHotkey(dataObject, countdownTimerData.startCountdownToTimeHotkeyId,
		   ("Ashmanix_Countdown_Timer_To_Time_Start_" +
		    (countdownTimerData.timerId).toStdString())
			   .c_str());

	SaveHotkey(dataObject, countdownTimerData.stopCountdownToTimeHotkeyId,
		   ("Ashmanix_Countdown_Timer_To_Time_Stop_" +
		    (countdownTimerData.timerId).toStdString())
			   .c_str());
}

void AshmanixTimer::LoadTimerWidgetDataFromOBSSaveData(
	obs_data_t *dataObject, TimerWidgetStruct *timerData)
{

	timerData->timerId = (char *)obs_data_get_string(dataObject, "timerId");
	timerData->shouldCountUp =
		(bool)obs_data_get_bool(dataObject, "shouldCountUp");
	timerData->showLeadingZero =
		(bool)obs_data_get_bool(dataObject, "showLeadingZero");
	timerData->selectedSource =
		(char *)obs_data_get_string(dataObject, "selectedSource");
	timerData->selectedScene =
		(char *)obs_data_get_string(dataObject, "selectedScene");
	timerData->endMessage =
		(char *)obs_data_get_string(dataObject, "endMessage");

	QDateTime savedTime = QDateTime::fromString(
		(char *)obs_data_get_string(dataObject, "dateTime"));
	QDateTime currentTime = QDateTime::currentDateTime();
	if (currentTime > savedTime) {
		savedTime = savedTime.addDays(1);
		if (currentTime > savedTime)
			savedTime = savedTime.addDays(1);
	}
	timerData->dateTime = savedTime;

	timerData->periodDays = (int)obs_data_get_int(dataObject, "periodDays");
	timerData->periodHours =
		(int)obs_data_get_int(dataObject, "periodHours");
	timerData->periodMinutes =
		(int)obs_data_get_int(dataObject, "periodMinutes");
	timerData->periodSeconds =
		(int)obs_data_get_int(dataObject, "periodSeconds");

	timerData->showDays = (bool)obs_data_get_bool(dataObject, "showDays");
	timerData->showHours = (bool)obs_data_get_bool(dataObject, "showHours");
	timerData->showMinutes =
		(bool)obs_data_get_bool(dataObject, "showMinutes");
	timerData->showSeconds =
		(bool)obs_data_get_bool(dataObject, "showSeconds");
	timerData->showEndMessage =
		(bool)obs_data_get_bool(dataObject, "showEndMessage");
	timerData->showEndScene =
		(bool)obs_data_get_bool(dataObject, "showEndScene");
	timerData->countdownTypeSelectedTab =
		(int)obs_data_get_int(dataObject, "countdownTypeSelectedTab");

	timerData->startCountdownHotkeyId =
		(int)obs_data_get_int(dataObject, "startCountdownHotkeyId");
	timerData->pauseCountdownHotkeyId =
		(int)obs_data_get_int(dataObject, "pauseCountdownHotkeyId");
	timerData->setCountdownHotkeyId =
		(int)obs_data_get_int(dataObject, "setCountdownHotkeyId");
	timerData->startCountdownToTimeHotkeyId = (int)obs_data_get_int(
		dataObject, "startCountdownToTimeHotkeyId");
	timerData->stopCountdownToTimeHotkeyId = (int)obs_data_get_int(
		dataObject, "stopCountdownToTimeHotkeyId");
}

QString AshmanixTimer::GetTimerID()
{
	return countdownTimerData.timerId;
}

QPushButton *AshmanixTimer::GetDeleteButton()
{
	return ui->deleteButton;
}

TimerWidgetStruct *AshmanixTimer::GetTimerData()
{
	return &countdownTimerData;
}

void AshmanixTimer::SetTimerData(TimerWidgetStruct newData)
{
	ui->timerNameLabel->setText(QString("Timer: %1").arg(newData.timerId));

	ui->dateTimeEdit->setDateTime(newData.dateTime);

	ui->timerDays->setText(QString::number(newData.periodDays));
	ui->timerHours->setText(QString::number(newData.periodHours));
	ui->timerMinutes->setText(QString::number(newData.periodMinutes));
	ui->timerSeconds->setText(QString::number(newData.periodSeconds));

	obs_log(LOG_INFO, "Timer %s tab set to %d",
		newData.timerId.toStdString().c_str(),
		newData.countdownTypeSelectedTab);

	ui->countdownTypeTabWidget->setCurrentIndex(
		newData.countdownTypeSelectedTab);

	countdownTimerData = newData;

	ui->timerNameLabel->setText(
		QString("Timer: %1").arg(countdownTimerData.timerId));
	InitialiseTimerTime();
}

bool AshmanixTimer::AlterTime(WebsocketRequestType requestType,
			      long long timeInMillis)
{
	bool result = false;

	if (ui->countdownTypeTabWidget->currentIndex() == 0) {
		switch (requestType) {
		case ADD_TIME:
			countdownTimerData.timeLeftInMillis += timeInMillis;
			break;
		case SET_TIME:
			countdownTimerData.timeLeftInMillis = timeInMillis;
			break;
			break;
		default:
			return false;
			break;
		}

		UpdateDateTimeDisplay(countdownTimerData.timeLeftInMillis);
		result = true;
	} else if (ui->countdownTypeTabWidget->currentIndex() == 1) {
		QDateTime updatedDateTime;

		switch (requestType) {
		case ADD_TIME:
			updatedDateTime = ui->dateTimeEdit->dateTime().addMSecs(
				timeInMillis);
			ui->dateTimeEdit->setDateTime(updatedDateTime);
			break;
		case SET_TIME:
			updatedDateTime = QDateTime::currentDateTime().addMSecs(
				timeInMillis);
			ui->dateTimeEdit->setDateTime(updatedDateTime);
			break;
		default:
			return false;
			break;
		}
		long long new_time = CalcToCurrentDateTimeInMillis(
			ui->dateTimeEdit->dateTime(), COUNTDOWNPERIOD);
		UpdateDateTimeDisplay(new_time);
		result = true;
	}
	emit RequestTimerReset();
	return result;
}

void AshmanixTimer::SetupTimerWidgetUI()
{
	ui->timerNameLabel->setText(
		QString("Timer: %1").arg(countdownTimerData.timerId));

	ui->settingsButton->setProperty("themeID", "propertiesIconSmall");
	ui->settingsButton->setText("");
	ui->settingsButton->setEnabled(true);
	ui->settingsButton->setToolTip(obs_module_text("AddTimerButtonTip"));

	ui->deleteButton->setProperty("themeID", "removeIconSmall");
	ui->deleteButton->setText("");
	ui->deleteButton->setEnabled(true);
	ui->deleteButton->setToolTip(obs_module_text("DeleteTimerButtonTip"));

	ui->timeDisplay->display(AshmanixTimer::ZEROSTRING);

	ui->dateTimeEdit->setMinimumDate(QDate::currentDate());
	ui->dateTimeEdit->setMaximumDate(QDate::currentDate().addDays(999));

	ui->timerDays->setMaxLength(3);
	ui->timerDays->setValidator(new QRegularExpressionValidator(
		QRegularExpression("^(0|[1-9]\\d{0,2})$"), this));
	ui->timerDays->setToolTip(obs_module_text("DaysCheckboxLabel"));

	ui->timerHours->setMaxLength(2);
	ui->timerHours->setValidator(new QRegularExpressionValidator(
		QRegularExpression("^(0?[0-9]|1[0-9]|2[0-3])$"), this));
	ui->timerHours->setToolTip(obs_module_text("HoursCheckboxLabel"));

	ui->timerMinutes->setMaxLength(2);
	ui->timerMinutes->setValidator(new QRegularExpressionValidator(
		QRegularExpression("^[1-5]?[0-9]"), this));
	ui->timerMinutes->setToolTip(obs_module_text("MinutesCheckboxLabel"));

	ui->timerSeconds->setAlignment(Qt::AlignCenter);
	ui->timerSeconds->setMaxLength(2);
	ui->timerSeconds->setValidator(new QRegularExpressionValidator(
		QRegularExpression("^[1-5]?[0-9]"), this));
	ui->timerSeconds->setToolTip(obs_module_text("SecondsCheckboxLabel"));

	ui->countdownTypeTabWidget->setTabText(
		0, obs_module_text("SetPeriodTabLabel"));
	ui->countdownTypeTabWidget->setTabText(
		1, obs_module_text("SetDatetimeTabLabel"));

	ui->countdownTypeTabWidget->setTabToolTip(
		0, obs_module_text("SetPeriodTabTip"));
	ui->countdownTypeTabWidget->setTabToolTip(
		1, obs_module_text("SetDatetimeTabTip"));

	ui->playButton->setProperty("themeID", "playIcon");
	ui->playButton->setEnabled(true);
	ui->playButton->setToolTip(obs_module_text("PlayButtonTip"));
	ui->pauseButton->setProperty("themeID", "pauseIcon");
	ui->pauseButton->setEnabled(false);
	ui->pauseButton->setToolTip(obs_module_text("PauseButtonTip"));
	ui->resetButton->setProperty("themeID", "restartIcon");
	ui->resetButton->setToolTip(obs_module_text("ResetButtonTip"));

	ui->toTimePlayButton->setProperty("themeID", "playIcon");
	ui->toTimePlayButton->setEnabled(true);
	ui->toTimePlayButton->setToolTip(
		obs_module_text("ToTimePlayButtonTip"));
	ui->toTimeStopButton->setProperty("themeID", "stopIcon");
	ui->toTimeStopButton->setEnabled(false);
	ui->toTimeStopButton->setToolTip(
		obs_module_text("ToTimeStopButtonTip"));

	countdownTimerData.isPlaying = false;
}

void AshmanixTimer::ConnectUISignalHandlers()
{
	QObject::connect(ui->playButton, SIGNAL(clicked()),
			 SLOT(PlayButtonClicked()));

	QObject::connect(ui->pauseButton, SIGNAL(clicked()),
			 SLOT(PauseButtonClicked()));

	QObject::connect(ui->resetButton, SIGNAL(clicked()),
			 SLOT(ResetButtonClicked()));

	QObject::connect(ui->toTimePlayButton, SIGNAL(clicked()),
			 SLOT(ToTimePlayButtonClicked()));

	QObject::connect(ui->toTimeStopButton, SIGNAL(clicked()),
			 SLOT(ToTimeStopButtonClicked()));

	QObject::connect(ui->deleteButton, SIGNAL(clicked()),
			 SLOT(DeleteButtonClicked()));

	QObject::connect(ui->settingsButton, SIGNAL(clicked()),
			 SLOT(SettingsButtonClicked()));

	QObject::connect(ui->countdownTypeTabWidget,
			 SIGNAL(currentChanged(int)),
			 SLOT(CountdownTypeTabChanged(int)));

	QObject::connect(ui->timerDays, SIGNAL(textEdited(QString)),
			 SLOT(DaysChanged(QString)));

	QObject::connect(ui->timerHours, SIGNAL(textEdited(QString)),
			 SLOT(HoursChanged(QString)));

	QObject::connect(ui->timerMinutes, SIGNAL(textEdited(QString)),
			 SLOT(MinutesChanged(QString)));

	QObject::connect(ui->timerSeconds, SIGNAL(textEdited(QString)),
			 SLOT(SecondsChanged(QString)));

	QObject::connect(ui->dateTimeEdit, SIGNAL(dateTimeChanged(QDateTime)),
			 SLOT(DateTimeChanged(QDateTime)));
}

QString
AshmanixTimer::ConvertDateTimeToFormattedDisplayString(long long timeInMillis,
						       bool showLeadingZero)
{

	QString formattedDateTimeString = GetFormattedTimerString(
		countdownTimerData.showDays, countdownTimerData.showHours,
		countdownTimerData.showMinutes, countdownTimerData.showSeconds,
		showLeadingZero, timeInMillis);

	return (formattedDateTimeString == "") ? "Nothing selected!"
					       : formattedDateTimeString;
}

void AshmanixTimer::StartTimerCounting()
{
	countdownTimerData.isPlaying = true;
	countdownTimerData.timer->start(COUNTDOWNPERIOD);
	ui->playButton->setEnabled(false);
	ui->pauseButton->setEnabled(true);
	ui->resetButton->setEnabled(false);

	ui->toTimePlayButton->setEnabled(false);
	ui->toTimeStopButton->setEnabled(true);

	ui->timerDays->setEnabled(false);
	ui->timerHours->setEnabled(false);
	ui->timerMinutes->setEnabled(false);
	ui->timerSeconds->setEnabled(false);

	ui->countdownTypeTabWidget->tabBar()->setEnabled(false);
	ui->dateTimeEdit->setEnabled(false);

	if (settingsDialogUi)
		settingsDialogUi->SetCountUpCheckBoxEnabled(false);

	SendTimerStateEvent(countdownTimerData.timerId, "started");
}

void AshmanixTimer::StopTimerCounting()
{
	countdownTimerData.isPlaying = false;
	countdownTimerData.timer->stop();
	ui->playButton->setEnabled(true);
	ui->pauseButton->setEnabled(false);
	ui->resetButton->setEnabled(true);

	ui->toTimePlayButton->setEnabled(true);
	ui->toTimeStopButton->setEnabled(false);

	ui->timerDays->setEnabled(true);
	ui->timerHours->setEnabled(true);
	ui->timerMinutes->setEnabled(true);
	ui->timerSeconds->setEnabled(true);

	ui->countdownTypeTabWidget->tabBar()->setEnabled(true);
	ui->dateTimeEdit->setEnabled(true);

	if (settingsDialogUi)
		settingsDialogUi->SetCountUpCheckBoxEnabled(true);

	SendTimerStateEvent(countdownTimerData.timerId, "stopped");
}

void AshmanixTimer::InitialiseTimerTime()
{
	countdownTimerData.timer = new QTimer();
	QObject::connect(countdownTimerData.timer, SIGNAL(timeout()),
			 SLOT(TimerAdjust()));

	countdownTimerData.timeLeftInMillis = GetMillisFromPeriodUI();
}

bool AshmanixTimer::IsSetTimeZero()
{
	bool isZero = false;

	if (countdownTimerData.timeLeftInMillis == 0) {
		isZero = true;
	} else if (ui->timerDays->text().toInt() == 0 &&
		   ui->timerHours->text().toInt() == 0 &&
		   ui->timerMinutes->text().toInt() == 0 &&
		   ui->timerSeconds->text().toInt() == 0) {
		isZero = true;
	}

	return isZero;
}

void AshmanixTimer::UpdateDateTimeDisplay(long long timeInMillis)
{
	ui->timeDisplay->display(ConvertMillisToDateTimeString(timeInMillis));
	QString formattedDisplayTime = ConvertDateTimeToFormattedDisplayString(
		timeInMillis, countdownTimerData.showLeadingZero);
	SetSourceText(formattedDisplayTime);
}

void AshmanixTimer::SetSourceText(QString newText)
{
	obs_source_t *selectedSource = obs_get_source_by_name(
		countdownTimerData.selectedSource.toStdString().c_str());

	if (selectedSource != NULL) {
		obs_data_t *sourceSettings =
			obs_source_get_settings(selectedSource);
		obs_data_set_string(sourceSettings, "text",
				    newText.toStdString().c_str());
		obs_source_update(selectedSource, sourceSettings);
		obs_data_release(sourceSettings);
		obs_source_release(selectedSource);
	}
}

void AshmanixTimer::SetCurrentScene()
{
	if (countdownTimerData.selectedScene.length()) {
		obs_source_t *source = obs_get_source_by_name(
			countdownTimerData.selectedScene.toStdString().c_str());
		if (source != NULL) {
			obs_frontend_set_current_scene(source);
			obs_source_release(source);
		}
	}
}

long long AshmanixTimer::GetMillisFromPeriodUI()
{
	long long days_ms =
		static_cast<long long>(ui->timerDays->text().toInt()) * 24 *
		60 * 60 * 1000;
	long long hours_ms =
		static_cast<long long>(ui->timerHours->text().toInt()) * 60 *
		60 * 1000;
	long long minutes_ms =
		static_cast<long long>(ui->timerMinutes->text().toInt()) * 60 *
		1000;
	long long seconds_ms =
		static_cast<long long>(ui->timerSeconds->text().toInt()) * 1000;

	return days_ms + hours_ms + minutes_ms + seconds_ms;
}

void AshmanixTimer::SendTimerTickEvent(QString timerId,
				       long long timeLeftInMillis)
{
	obs_data_t *eventData = obs_data_create();

	// Convert milliseconds to readable format
	QString timeString = ConvertDateTimeToFormattedDisplayString(
		timeLeftInMillis, countdownTimerData.showLeadingZero);

	obs_data_set_string(eventData, "timer_id",
			    timerId.toStdString().c_str());
	obs_data_set_string(eventData, "time_display",
			    timeString.toStdString().c_str());
	obs_data_set_int(eventData, "time_left_ms", timeLeftInMillis);

	emit RequestSendWebsocketEvent("const char *eventName", eventData);
	obs_data_release(eventData);
}

void AshmanixTimer::SendTimerStateEvent(QString timerId, const char *state)
{
	obs_data_t *eventData = obs_data_create();
	obs_data_set_string(eventData, "timer_id",
			    timerId.toStdString().c_str());
	obs_data_set_string(eventData, "state", state);

	if (countdownTimerData.selectedSource.length() > 0) {
		obs_data_set_string(
			eventData, "text_source",
			countdownTimerData.selectedSource.toStdString().c_str());
	}

	emit RequestSendWebsocketEvent("timer_state_changed", eventData);
	obs_data_release(eventData);
}

void AshmanixTimer::SaveHotkey(obs_data_t *sv_data, obs_hotkey_id id,
			       const char *name)
{
	if ((int)id == -1)
		return;
	OBSDataArrayAutoRelease array = obs_hotkey_save(id);
	obs_data_set_array(sv_data, name, array);
};

void AshmanixTimer::PressPlayButton()
{
	ui->playButton->click();
}
void AshmanixTimer::PressResetButton()
{
	ui->resetButton->click();
}
void AshmanixTimer::PressStopButton()
{
	ui->pauseButton->click();
}
void AshmanixTimer::PressToTimePlayButton()
{
	ui->toTimePlayButton->click();
}
void AshmanixTimer::PressToTimeStopButton()
{
	ui->toTimeStopButton->click();
}

// --------------------------------- Public Slots ----------------------------------

void AshmanixTimer::PlayButtonClicked()
{
	if (ui->countdownTypeTabWidget->currentIndex() == 1) {
		ui->countdownTypeTabWidget->setCurrentIndex(0);
	}

	if ((!countdownTimerData.shouldCountUp && IsSetTimeZero()) ||
	    (countdownTimerData.shouldCountUp &&
	     countdownTimerData.timeLeftInMillis >= GetMillisFromPeriodUI()))
		return;

	ui->timeDisplay->display(ConvertMillisToDateTimeString(
		countdownTimerData.timeLeftInMillis));
	StartTimerCounting();
}

void AshmanixTimer::PauseButtonClicked()
{
	if (ui->countdownTypeTabWidget->currentIndex() == 1) {
		ui->countdownTypeTabWidget->setCurrentIndex(0);
	}

	StopTimerCounting();
	SendTimerStateEvent(countdownTimerData.timerId, "paused");
}

void AshmanixTimer::ResetButtonClicked()
{
	if (ui->countdownTypeTabWidget->currentIndex() == 1) {
		ui->countdownTypeTabWidget->setCurrentIndex(0);
	}

	StopTimerCounting();
	countdownTimerData.shouldCountUp
		? countdownTimerData.timeLeftInMillis = 0
		: countdownTimerData.timeLeftInMillis = GetMillisFromPeriodUI();

	UpdateDateTimeDisplay(countdownTimerData.timeLeftInMillis);
}

void AshmanixTimer::ToTimePlayButtonClicked()
{
	if (ui->countdownTypeTabWidget->currentIndex() == 0) {
		ui->countdownTypeTabWidget->setCurrentIndex(1);
	}

	if (countdownTimerData.shouldCountUp) {
		countdownTimerData.timeToCountUpToStart =
			QDateTime::currentDateTime();
		countdownTimerData.timeLeftInMillis = 0;
	} else {
		countdownTimerData.timeLeftInMillis =
			CalcToCurrentDateTimeInMillis(
				ui->dateTimeEdit->dateTime(), COUNTDOWNPERIOD);
	}

	ui->timeDisplay->display(ConvertMillisToDateTimeString(
		countdownTimerData.timeLeftInMillis));
	StartTimerCounting();
}

void AshmanixTimer::ToTimeStopButtonClicked()
{
	if (ui->countdownTypeTabWidget->currentIndex() == 0) {
		ui->countdownTypeTabWidget->setCurrentIndex(1);
	}

	StopTimerCounting();
}

// ------------------------------- Private Slots ----------------------------------

void AshmanixTimer::CountdownTypeTabChanged(int index)
{
	countdownTimerData.countdownTypeSelectedTab = index;
}

void AshmanixTimer::SettingsButtonClicked()
{
	obs_log(LOG_INFO, "Settings button clicked for Timer %s",
		(countdownTimerData.timerId).toStdString().c_str());
	if (!settingsDialogUi) {
		settingsDialogUi =
			new SettingsDialog(this, &countdownTimerData);
	}
	if (settingsDialogUi->isVisible()) {
		settingsDialogUi->raise();
		settingsDialogUi->activateWindow();
	} else {
		settingsDialogUi->setVisible(true);
	}
}

void AshmanixTimer::DeleteButtonClicked()
{
	emit RequestDelete(countdownTimerData.timerId);
}

void AshmanixTimer::TimerAdjust()
{
	// Flag for ending timer
	bool endTimer = false;
	bool isCountingDown = !countdownTimerData.shouldCountUp;
	long long timerPeriodMillis = countdownTimerData.timeLeftInMillis;

	if (isCountingDown) {
		// Counting down
		if (ui->countdownTypeTabWidget->currentIndex() == 0) {
			// If selected tab is period
			timerPeriodMillis -= COUNTDOWNPERIOD;
		} else {
			// If selected tab is datetime
			timerPeriodMillis = CalcToCurrentDateTimeInMillis(
				ui->dateTimeEdit->dateTime(), COUNTDOWNPERIOD);
		}
		if (timerPeriodMillis < COUNTDOWNPERIOD)
			endTimer = true;
	} else {
		// When counting up always add to current timer

		// Check if we need to end timer
		if (ui->countdownTypeTabWidget->currentIndex() == 0) {
			timerPeriodMillis += COUNTDOWNPERIOD;
			// If selected tab is period
			if (timerPeriodMillis >= GetMillisFromPeriodUI())
				endTimer = true;
		} else {
			timerPeriodMillis =
				countdownTimerData.timeToCountUpToStart.msecsTo(
					QDateTime::currentDateTime());
			// If selected tab is datetime
			if ((countdownTimerData.timeToCountUpToStart.msecsTo(
				    ui->dateTimeEdit->dateTime())) -
				    timerPeriodMillis <=
			    COUNTDOWNPERIOD)
				endTimer = true;
		}
	}

	countdownTimerData.timeLeftInMillis = timerPeriodMillis;
	UpdateDateTimeDisplay(countdownTimerData.timeLeftInMillis);

	// Send tick event
	SendTimerTickEvent(countdownTimerData.timerId,
			   countdownTimerData.timeLeftInMillis);

	if (endTimer == true) {
		if (countdownTimerData.showEndMessage) {
			SetSourceText(
				countdownTimerData.endMessage.toStdString()
					.c_str());
		}
		if (countdownTimerData.showEndScene) {
			SetCurrentScene();
		}
		if (isCountingDown) {
			ui->timeDisplay->display(AshmanixTimer::ZEROSTRING);
			countdownTimerData.timeLeftInMillis = 0;
		} else {
			if (ui->countdownTypeTabWidget->currentIndex() == 0) {
				countdownTimerData.timeLeftInMillis =
					GetMillisFromPeriodUI();
			} else {
				countdownTimerData.timeLeftInMillis =
					countdownTimerData.timeToCountUpToStart
						.msecsTo(ui->dateTimeEdit
								 ->dateTime());
			}
			UpdateDateTimeDisplay(
				countdownTimerData.timeLeftInMillis);
		}
		// Send completion event
		SendTimerStateEvent(countdownTimerData.timerId, "completed");
		StopTimerCounting();
		return;
	}
}

void AshmanixTimer::HandleTimerReset()
{
	countdownTimerData.timer->stop();
	countdownTimerData.timer->start(COUNTDOWNPERIOD);
}

void AshmanixTimer::DaysChanged(QString newText)
{
	countdownTimerData.periodDays = newText.toInt();
}

void AshmanixTimer::HoursChanged(QString newText)
{
	countdownTimerData.periodHours = newText.toInt();
}

void AshmanixTimer::MinutesChanged(QString newText)
{
	countdownTimerData.periodMinutes = newText.toInt();
}

void AshmanixTimer::SecondsChanged(QString newText)
{
	countdownTimerData.periodSeconds = newText.toInt();
}

void AshmanixTimer::DateTimeChanged(QDateTime newDateTime)
{
	countdownTimerData.dateTime = newDateTime;
}
