

#ifndef INTERFACEFORSCORM_H
#define INTERFACEFORSCORM_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

#include "../lua/env.h"

#include <osg/Timer>

class keyValue
{
public:
	std::string key;
	std::string value;
};

class interfaceForScorm
{
public:
    interfaceForScorm();

    //Сохранение в выходной файл для передачи в LMS систему через SCORM.output -> JAVA-applet -> javaScript
    void SetDataToLMS();
    //Загрузка данных, полученых из LMS системы через javaScript -> java applet -> SCORM.input
    void GetDataFromLMS();

    //Добавление новой записи
    void SetValue (std::string key, std::string value);
    //Возврат значения из LMS по имени поля (например cmi.learner_name)
    std::string GetValue(std::string key);


    //дополнительные сервисные функции
    int strToInt(std::string);
    float strToFloat(std::string strIntString);
    std::string floatToStr(float value);
    std::string intToStr(int value);

private:
	void SetDataToFile(std::string file);


    //модель данных SCORM
    //Записи полученные из LMS (пара ключ-значение)
    std::map <std::string, std::string> DataFromLMT;
    //Записи передаваемые в LMS (пара ключ-значение)
    std::vector <keyValue> DataToLMT;

	bool find (std::string);

    //for time
    osg::Timer_t tick1;


};

#endif // INTERFACEFORSCORM_H


/*
http://scorm.com/scorm-explained/technical-scorm/run-time/run-time-reference/
Run-Time Reference
SCORM 2004 4th Edition
API Signature
Initialize( “” ) : bool – Begins a communication session with the LMS.
Terminate( “” ) : bool – Ends a communication session with the LMS.
GetValue( element : CMIElement ) : string – Retrieves a value from the LMS.
SetValue( element : CMIElement, value : string) : string – Saves a value to the LMS.
Commit( “” ) : bool – Indicates to the LMS that all data should be persisted (not required).
GetLastError() : CMIErrorCode – Returns the error code that resulted from the last API call.
GetErrorString( errorCode : CMIErrorCode ) : string – Returns a short string describing the specified error code.
GetDiagnostic( errorCode : CMIErrorCode ) : string – Returns detailed information about the last error that occurred.
Data Model
cmi._version (characterstring, RO) Represents the version of the data model
cmi.comments_from_learner._children (comment,location,timestamp, RO) Listing of supported data model elements
cmi.comments_from_learner._count (non-negative integer, RO) Current number of learner comments
cmi.comments_from_learner.n.comment (localized_string_type (SPM: 4000), RW) Textual input
cmi.comments_from_learner.n.location (characterstring (SPM: 250), RW) Point in the SCO to which the comment applies
cmi.comments_from_learner.n.timestamp (time (second,10,0), RW) Point in time at which the comment was created or most recently changed
cmi.comments_from_lms._children (comment,location,timestamp, RO) Listing of supported data model elements
cmi.comments_from_lms._count (non-negative integer, RO) Current number of comments from the LMS
cmi.comments_from_lms.n.comment (localized_string_type (SPM: 4000), RO) Comments or annotations associated with a SCO
cmi.comments_from_lms.n.location (characterstring (SPM: 250), RO) Point in the SCO to which the comment applies
cmi.comments_from_lms.n.timestamp (time(second,10,0), RO) Point in time at which the comment was created or most recently changed
cmi.completion_status (state (completed, incomplete, not attempted, unknown), RW) Indicates whether the learner has completed the SCO
cmi.completion_threshold (real(10,7) range (0..1), RO) Used to determine whether the SCO should be considered complete
cmi.credit (state (credit, no_credit), RO) Indicates whether the learner will be credited for performance in the SCO
cmi.entry (state (ab_initio, resume, “”), RO) Asserts whether the learner has previously accessed the SCO
cmi.exit (state (timeout, suspend, logout, normal, “”), WO) Indicates how or why the learner left the SCO
cmi.interactions._children (id,type,objectives,timestamp,correct_responses,weighting,learner_response,result,latency,description, RO)Listing of supported data model elements
cmi.interactions._count (non-negative integer, RO) Current number of interactions being stored by the LMS
cmi.interactions.n.id (long_identifier_type (SPM: 4000), RW) Unique label for the interaction
cmi.interactions.n.type (state (true_false, multiple_choice, fill_in, long_fill_in, matching, performance, sequencing, likert, numeric, other), RW) Which type of interaction is recorded
cmi.interactions.n.objectives._count (non-negative integer, RO) Current number of objectives (i.e., objective identifiers) being stored by the LMS for this interaction
cmi.interactions.n.objectives.n.id (long_identifier_type (SPM: 4000), RW) Label for objectives associated with the interaction
cmi.interactions.n.timestamp (time(second,10,0), RW) Point in time at which the interaction was first made available to the learner for learner interaction and response
cmi.interactions.n.correct_responses._count (non-negative integer, RO) Current number of correct responses being stored by the LMS for this interaction
cmi.interactions.n.correct_responses.n.pattern (format depends on interaction type, RW) One correct response pattern for the interaction
cmi.interactions.n.weighting (real (10,7), RW) Weight given to the interaction relative to other interactions
cmi.interactions.n.learner_response (format depends on interaction type, RW) Data generated when a learner responds to an interaction
cmi.interactions.n.result (state (correct, incorrect, unanticipated, neutral, real (10,7) ), RW) Judgment of the correctness of the learner response
cmi.interactions.n.latency (timeinterval (second,10,2), RW) Time elapsed between the time the interaction was made available to the learner for response and the time of the first response
cmi.interactions.n.description (localized_string_type (SPM: 250), RW) Brief informative description of the interaction
cmi.launch_data (characterstring (SPM: 4000), RO) Data provided to a SCO after launch, initialized from the dataFromLMS manifest element
cmi.learner_id (long_identifier_type (SPM: 4000), RO) Identifies the learner on behalf of whom the SCO was launched
cmi.learner_name (localized_string_type (SPM: 250), RO) Name provided for the learner by the LMS
cmi.learner_preference._children (audio_level,language,delivery_speed,audio_captioning, RO) Listing of supported data model elements
cmi.learner_preference.audio_level (real(10,7), range (0..*), RW) Specifies an intended change in perceived audio level
cmi.learner_preference.language (language_type (SPM 250), RW) The learner’s preferred language for SCOs with multilingual capability
cmi.learner_preference.delivery_speed (real(10,7), range (0..*), RW) The learner’s preferred relative speed of content delivery
cmi.learner_preference.audio_captioning (state (-1,0,1), RW) Specifies whether captioning text corresponding to audio is displayed
cmi.location (characterstring (SPM: 1000), RW) The learner’s current location in the SCO
cmi.max_time_allowed (timeinterval (second,10,2), RO) Amount of accumulated time the learner is allowed to use a SCO
cmi.mode (state (browse, normal, review), RO) Identifies one of three possible modes in which the SCO may be presented to the learner
cmi.objectives._children (id,score,success_status,completion_status,progress_measure,description, RO) Listing of supported data model elements
cmi.objectives._count (non-negative integer, RO) Current number of objectives being stored by the LMS
cmi.objectives.n.id (long_identifier_type (SPM: 4000), RW) Unique label for the objective
cmi.objectives.n.score._children (scaled,raw,min,max, RO) Listing of supported data model elements
cmi.objectives.n.score.scaled (real (10,7) range (-1..1), RW) Number that reflects the performance of the learner for the objective
cmi.objectives.n.score.raw (real (10,7), RW) Number that reflects the performance of the learner, for the objective, relative to the range bounded by the values of min and max
cmi.objectives.n.score.min (real (10,7), RW) Minimum value, for the objective, in the range for the raw score
cmi.objectives.n.score.max (real (10,7), RW) Maximum value, for the objective, in the range for the raw score
cmi.objectives.n.success_status (state (passed, failed, unknown), RW) Indicates whether the learner has mastered the objective
cmi.objectives.n.completion_status (state (completed, incomplete, not attempted, unknown), RW) Indicates whether the learner has completed the associated objective
cmi.objectives.n.progress_measure (real (10,7) range (0..1), RW) Measure of the progress the learner has made toward completing the objective
cmi.objectives.n.description (localized_string_type (SPM: 250), RW) Provides a brief informative description of the objective
cmi.progress_measure (real (10,7) range (0..1), RW) Measure of the progress the learner has made toward completing the SCO
cmi.scaled_passing_score (real(10,7) range (-1 .. 1), RO) Scaled passing score required to master the SCO
cmi.score._children (scaled,raw,min,max, RO) Listing of supported data model elements
cmi.score.scaled (real (10,7) range (-1..1), RW) Number that reflects the performance of the learner
cmi.score.raw (real (10,7), RW) Number that reflects the performance of the learner relative to the range bounded by the values of min and max
cmi.score.min (real (10,7), RW) Minimum value in the range for the raw score
cmi.score.max (real (10,7), RW) Maximum value in the range for the raw score
cmi.session_time (timeinterval (second,10,2), WO) Amount of time that the learner has spent in the current learner session for this SCO
cmi.success_status (state (passed, failed, unknown), RW) Indicates whether the learner has mastered the SCO
cmi.suspend_data (characterstring (SPM: 64000), RW) Provides space to store and retrieve data between learner sessions
cmi.time_limit_action (state (exit,message, continue,message, exit,no message, continue,no message), RO) Indicates what the SCO should do when cmi.max_time_allowed is exceeded
cmi.total_time (timeinterval (second,10,2), RO) Sum of all of the learner’s session times accumulated in the current learner attempt
adl.nav.request (request(continue, previous, choice, jump, exit, exitAll, abandon, abandonAll, suspendAll _none_), RW) Navigation request to be processed immediately following Terminate()
adl.nav.request_valid.continue (state (true, false, unknown), RO) Used by a SCO to determine if a Continue navigation request will succeed.
adl.nav.request_valid.previous (state (true, false, unknown), RO) Used by a SCO to determine if a Previous navigation request will succeed.
adl.nav.request_valid.choice.{target=} (state (true, false, unknown), RO) Used by a SCO to determine if a Choice navigation request for the target activity will succeed.
adl.nav.request_valid.jump.{target=} (state (true, false, unknown), RO) Used by a SCO to determine if a Jump navigation request for the target activity will succeed.
Error Codes
No Error (0) No error occurred, the previous API call was successful.
General Exception (101) No specific error code exists to describe the error. Use GetDiagnostic for more information.
General Initialization Failure (102) Call to Initialize failed for an unknown reason.
Already Initialized (103) Call to Initialize failed because Initialize was already called.
Content Instance Terminated (104) Call to Initialize failed because Terminate was already called.
General Termination Failure (111) Call to Terminate failed for an unknown reason.
Termination Before Initialization (112) Call to Terminate failed because it was made before the call to Initialize.
Termination After Termination (113) Call to Terminate failed because Terminate was already called.
Retrieve Data Before Initialization (122) Call to GetValue failed because it was made before the call to Initialize.
Retrieve Data After Termination (123) Call to GetValue failed because it was made after the call to Terminate.
Store Data Before Initialization (132) Call to SetValue failed because it was made before the call to Initialize.
Store Data After Termination (133) Call to SetValue failed because it was made after the call to Terminate.
Commit Before Initialization (142) Call to Commit failed because it was made before the call to Initialize.
Commit After Termination (143) Call to Commit failed because it was made after the call to Terminate.
General Argument Error (201) An invalid argument was passed to an API method (usually indicates that Initialize, Commit or Terminate did not receive the expected empty string argument.
General Get Failure (301) Indicates a failed GetValue call where no other specific error code is applicable. Use GetDiagnostic for more information.
General Set Failure (351) Indicates a failed SetValue call where no other specific error code is applicable. Use GetDiagnostic for more information.
General Commit Failure (391) Indicates a failed Commit call where no other specific error code is applicable. Use GetDiagnostic for more information.
Undefined Data Model Element (401) The data model element name passed to GetValue or SetValue is not a valid SCORM data model element.
Unimplemented Data Model Element (402) The data model element indicated in a call to GetValue or SetValue is valid, but was not implemented by this LMS. In SCORM 2004, this error would indicate an LMS that is not fully SCORM conformant.
Data Model Element Value Not Initialized (403) Attempt to read a data model element that has not been initialized by the LMS or through a SetValue call. This error condition is often reached during normal execution of a SCO.
Data Model Element Is Read Only (404) SetValue was called with a data model element that can only be read.
Data Model Element Is Write Only (405) GetValue was called on a data model element that can only be written to.
Data Model Element Type Mismatch (406) SetValue was called with a value that is not consistent with the data format of the supplied data model element.
Data Model Element Value Out Of Range (407) The numeric value supplied to a SetValue call is outside of the numeric range allowed for the supplied data model element.
Data Model Dependency Not Established (408) Some data model elements cannot be set until another data model element was set. This error condition indicates that the prerequisite element was not set before the dependent element.

  */

/*
Фактически взаимодействие между SCO заключается в получении и изменении значений ряда определенных стандартом «переменных», например:
 +cmi.completion_status – статус выполнения SCO;
 +cmi.progress_measure – количество (или процент от максимального количества) выполненных заданий или действий в рамках SCO;
++cmi.completion_threshold – минимально-необходимое значение (количество  или минимальный процент) выполненных заданий или действий для принятия решения о статусе выполнения SCO;
 +cmi.success_status – статус успешности;
 +cmi.score_scaled -нормализованные баллы (%);
++cmi.score.min — минимальная оценка;
++cmi.score.max — максимальная оценка;
++cmi.scaled_passing_score – нормализованный проходной балл (%);
 +cmi.score.raw – оценка в баллах;
x cmi.location – маркер выхода (для «закладок» и контрольных точек);
x cmi.entry – маркер входа (для «закладок» и контрольных точек);
x cmi.comments_from_learner.n.comment – комментарий пользователя;
++cmi.suspend_data – сохранение данных между сеансами (обучаемый приостановил изучение с последующим его возобновлением);
 +cmi.session_time – время текущей сессии;
x cmi.total_time – общее время изучения;
+ cmi.max_time_allowed – ограничение по времени;
+ cmi.learner_name – имя студента;
+ cmi.learner_id – ID студента в системе, и т.д.

Для передачи LMS-системе «эффективности» выполнения scorm-пакета используется три группы переменных.
    Информация о количестве выполненных заданий или действий в рамках SCO и общая оценка статуса выполнения (оценивается количество , а не  правильность):
Переменная cmi.progress_measure определяет количество или процент выполненных учебных задач. Например, при выполнении имитатора обучаемый обнаружил 9 неисправностей из 10 возможных. В таком случае можно передать в LMS-систему значения «9» (абсолютная шкала) или «0.9» (шкала 0..1) или  «90» (%), в зависимости от принятой разработчиком\заказчиком шкалы оценок. Значение  cmi.progress_measure влияет на  общий статус выполнения SCO (cmi.completion_status).
Переменная cmi.completion_threshold определяет минимально-необходимое значение cmi.progress_measure (см. выше), при котором автоматически (в LMS-системе) принимается решения о статусе выполнения SCO.
Переменная cmi.completion_status определяет  статус выполнения SCO. При этом оценивается количество действий, а не их правильность. Например обучаемый может выполнить 10 из 10 заданий, но все неправильно. Если задана переменная порогового значения (cmi.completion_threshold), то оценка выполняется автоматически, иначе, разработчик должен определить самостоятельно. Возможны четыре значения: “completed” - успешное выполнение, “incomplete” - недостаточное количество выполненных заданий; “not attempted” - обучаемый не выполнял задания; “unknown” - отсутствие утверждения о статусе выполнения SCO.

    Информация о правильности выполненных заданий или действий в рамках SCO и общая оценка статуса правильности выполнения (фактически общая оценка успешности):
Переменная  cmi.score.raw определяет не нормализованную оценку (баллы), т. е. не заданы минимальный и максимальный уровень баллов.
Переменная cmi.score_scaled пределяет оценку в нормализованных баллах  (%). При этом минимальный и максимальный уровень баллов (диапазон) задается переменными cmi.score.min и cmi.score.max.
Переменная cmi.scaled_passing_score определяет нормализованный проходной балл (%), позволяющий LMS-системе определять статус успешности (cmi.success_status);
Переменная cmi.success_status определяет  статус успешности выполнения SCO. Если задана переменная проходного балла (cmi.scaled_passing_score), то оценка выполняется автоматически, иначе, разработчик должен определить самостоятельно. Возможны четыре значения: “passed” - правильное выполнение, “failed” - неправильное выполненных заданий; “unknown” - отсутствие утверждения о правильности выполнения SCO.

    Дополнительная информация о выполнении SCO определяется следующими переменными:
Переменная  cmi.session_time определяет время, затраченное обучаемым на выполнение SCO. Сумма времени всех попыток выполнения автоматически суммируется LMS-системой в переменной cmi.total_time;
Переменная  cmi.exit определяет флаг завершения работы обучаемого с SCO. Возможны четыре значения: “time-out” - превышение лимита времени, установленного для данного SCO LMS-системой (переменная cmi.max_time_allowed и  cmi.time_limit_action); “suspend” - обучаемый приостановил изучение SCO с возможностью продолжить в дальнейшем; “logout” - выход из LMS-системы; “normal” -  нормальный выход по завершению SCO; “” (пустая строка, значение по умолчанию) — условия выхода не определены.
*/
