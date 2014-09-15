#include "taskFlyport.h"
#include "grovelib.h"

#define COUNT_250MS 10

char MY_EMAIL[50];
char MY_EMAIL_USER[50];
char MY_EMAIL_PASS[30];
char MY_SMTP[50];
char MY_SMTP_PORT[10];
char EMAIL_DEST[50];
char EMAIL_SUBJECT[100];
char EMAIL_BODY[200];

char GMT[10];
char start_string[10];
char stop_string[10];

BOOL ParamSet = FALSE;
char dbg_string[100];	
WORD report;

struct tm start_alarm_time;
struct tm stop_alarm_time;
int timer;

void stop_alarm();

void  start_alarm() //function called when the alarm is rised
{
	RTCCAlarmConf(&stop_alarm_time,REPEAT_INFINITE,EVERY_DAY,stop_alarm);//set alarm
    RTCCAlarmSet(ON);
	timer=1;
}

void  stop_alarm() //function called when the alarm is rised
{
	RTCCAlarmConf(&start_alarm_time,REPEAT_INFINITE,EVERY_DAY,start_alarm);//set alarm
    RTCCAlarmSet(ON);
	timer=0; 
	WFSleep();
}

void FlyportTask()	
{	
	DWORD i=0;
	unsigned int cnt_ee_i=0;
	unsigned int cnt_ee_d=11;
	char ee_i_d[2];
	vTaskDelay(100);
	
	float anVal = 0.0;
	float anVal_zero = 70.0;
	
	time_t now;
	struct tm *ts;
	DWORD epoch=0;
	DWORD epochtime=0xA2C2A;
	int GMT_hour_adding;
	
	timer = 0;
	
	EEInit(EE_ADDR_DEF, HIGH_SPEED, EE_SIZE_DEF);
	
	// GROVE board
    void *board = new(IoT_Board);
	
	// GROVE device	
	void *light_sensor = new (An_i);
	attachToBoard(board, light_sensor, AN1);
	
	// Connection to Network
	if (WFCustomExist())
	{
		WFCustomLoad();
		UARTWrite(1,"Connecting to custom...\r\n");
		WFConnect(WF_CUSTOM);
	}
	else
	{
		UARTWrite(1,"Connecting to default...\r\n");
		WFConnect(WF_DEFAULT);
	}
	while (WFStatus != CONNECTED);
	UARTWrite(1,"Flyport connected... hello world!\r\n");
	vTaskDelay(200);
	
	PWMInit(1,1,0);
	PWMOn(LED2_Pin,1);
	
	if(AppConfig.networkType==WF_INFRASTRUCTURE&&WFStatus == CONNECTED)
	{
		IOPut(LED1_Pin, 1);
		
		/* 1 */
		EELoadData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
		EELoadString(cnt_ee_d, MY_EMAIL, ee_i_d[0]);
		MY_EMAIL[(int)ee_i_d[0]]='\0';
		cnt_ee_i++;
		cnt_ee_d += ee_i_d[0];
		/* 2 */
		EELoadData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
		EELoadString(cnt_ee_d, MY_EMAIL_USER, ee_i_d[0]);
		MY_EMAIL_USER[(int)ee_i_d[0]]='\0';
		cnt_ee_i++;
		cnt_ee_d += ee_i_d[0];
		/* 3 */
		EELoadData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
		EELoadString(cnt_ee_d, MY_EMAIL_PASS, ee_i_d[0]);
		MY_EMAIL_PASS[(int)ee_i_d[0]]='\0';
		cnt_ee_i++;
		cnt_ee_d += ee_i_d[0];
		/* 4 */
		EELoadData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
		EELoadString(cnt_ee_d, MY_SMTP, ee_i_d[0]);
		MY_SMTP[(int)ee_i_d[0]]='\0';
		cnt_ee_i++;
		cnt_ee_d += ee_i_d[0];
		/* 5 */
		EELoadData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
		EELoadString(cnt_ee_d, MY_SMTP_PORT, ee_i_d[0]);
		MY_SMTP_PORT[(int)ee_i_d[0]]='\0';
		cnt_ee_i++;
		cnt_ee_d += ee_i_d[0];
		/* 6 */
		EELoadData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
		EELoadString(cnt_ee_d, EMAIL_SUBJECT, ee_i_d[0]);
		EMAIL_SUBJECT[(int)ee_i_d[0]]='\0';
		cnt_ee_i++;
		cnt_ee_d += ee_i_d[0];
		/* 7 */
		EELoadData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
		EELoadString(cnt_ee_d, EMAIL_BODY, ee_i_d[0]);
		EMAIL_BODY[(int)ee_i_d[0]]='\0';
		cnt_ee_i++;
		cnt_ee_d += ee_i_d[0];
		/* 8 */
		EELoadData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
		EELoadString(cnt_ee_d, EMAIL_DEST, ee_i_d[0]);
		EMAIL_DEST[(int)ee_i_d[0]]='\0';
		cnt_ee_i++;
		cnt_ee_d += ee_i_d[0];
		/* 9 */
		EELoadData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
		if(ee_i_d[0]!=0)
		{
			EELoadString(cnt_ee_d, start_string, ee_i_d[0]);
			start_string[(int)ee_i_d[0]]='\0';
			cnt_ee_i++;
			cnt_ee_d += ee_i_d[0];
			/* 10 */
			EELoadData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
			EELoadString(cnt_ee_d, stop_string, ee_i_d[0]);
			stop_string[(int)ee_i_d[0]]='\0';
			cnt_ee_i++;
			cnt_ee_d += ee_i_d[0];
			/* 11 */
			EELoadData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
			EELoadString(cnt_ee_d, GMT, ee_i_d[0]);
			GMT[(int)ee_i_d[0]]='\0';
			
			GMT_hour_adding = atoi(GMT);
			
			dbg_string[0]=start_string[0];
			dbg_string[1]=start_string[1];
			dbg_string[2]='\0';
			start_alarm_time.tm_hour=atoi(dbg_string);
			
			dbg_string[0]=start_string[3];
			dbg_string[1]=start_string[4];			
			start_alarm_time.tm_min=atoi(dbg_string);
			
			dbg_string[0]=stop_string[0];
			dbg_string[1]=stop_string[1];
			stop_alarm_time.tm_hour=atoi(dbg_string);
			
			dbg_string[0]=stop_string[3];
			dbg_string[1]=stop_string[4];
			stop_alarm_time.tm_min=atoi(dbg_string);

			while(epoch<epochtime)
			{
				vTaskDelay(50);
				//Remember to enable the SNTP Client in the wizard.
				epoch=SNTPGetUTCSeconds();
			}
			now=(time_t)epoch;
			ts=localtime(&now);
			vTaskDelay(20);
			ts->tm_hour = (ts->tm_hour + GMT_hour_adding);
			// Correct if overflowed hour 0-24 format
			if(ts->tm_hour > 24)
				ts->tm_hour = ts->tm_hour - 24;
			else if(ts->tm_hour < 0)
				ts->tm_hour = ts->tm_hour +24;
			sprintf (dbg_string, "\nReceived date/time is: %s \r\n", asctime (ts) );
			_dbgwrite(dbg_string);
			RTCCSet(ts);
			vTaskDelay(50);
			
			/* da togliere */
			start_alarm_time.tm_min = start_alarm_time.tm_min+2;
			
			RTCCAlarmConf(&start_alarm_time,REPEAT_INFINITE,EVERY_DAY,start_alarm);
			RTCCAlarmSet(ON);
			vTaskDelay(100);
			WFSleep();
		}
		else
		{
			timer=1;
			vTaskDelay(100);
			WFHibernate();
		}
	}
	
	while(1)
	{
		// routine to saving new network parameters
		if (ParamSet)
		{
			UARTWrite(1, "Params changed!\r\n");
			WFDisconnect();
			while (WFStatus != NOT_CONNECTED);
			vTaskDelay(50);
			WFCustomSave();
			UARTWrite(1,"Custom config saved!\r\n");
			UARTWrite(1,"Connecting custom...\r\n");
			WFConnect(WF_CUSTOM);
			while (WFStatus != CONNECTED);
			vTaskDelay(250);
			
			/*save to eeprom*/
			/* 1 */
			EESaveString(cnt_ee_d, MY_EMAIL, strlen(MY_EMAIL));
			cnt_ee_d += strlen(MY_EMAIL);
			ee_i_d[0] = strlen(MY_EMAIL);
			EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
			cnt_ee_i++;
			/* 2 */
			EESaveString(cnt_ee_d, MY_EMAIL_USER, strlen(MY_EMAIL_USER));
			cnt_ee_d += strlen(MY_EMAIL_USER);
			ee_i_d[0] = strlen(MY_EMAIL_USER);
			EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
			cnt_ee_i++;
			/* 3 */
			EESaveString(cnt_ee_d, MY_EMAIL_PASS, strlen(MY_EMAIL_PASS));
			cnt_ee_d += strlen(MY_EMAIL_PASS);
			ee_i_d[0] = strlen(MY_EMAIL_PASS);
			EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
			cnt_ee_i++;
			/* 4 */
			EESaveString(cnt_ee_d, MY_SMTP, strlen(MY_SMTP));
			cnt_ee_d += strlen(MY_SMTP);
			ee_i_d[0] = strlen(MY_SMTP);
			EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
			cnt_ee_i++;
			/* 5 */
			EESaveString(cnt_ee_d, MY_SMTP_PORT, strlen(MY_SMTP_PORT));
			cnt_ee_d += strlen(MY_SMTP_PORT);
			ee_i_d[0] = strlen(MY_SMTP_PORT);
			EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
			cnt_ee_i++;
			/* 6 */
			EESaveString(cnt_ee_d, EMAIL_SUBJECT, strlen(EMAIL_SUBJECT));
			cnt_ee_d += strlen(EMAIL_SUBJECT);
			ee_i_d[0] = strlen(EMAIL_SUBJECT);
			EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
			cnt_ee_i++;
			/* 7 */
			EESaveString(cnt_ee_d, EMAIL_BODY, strlen(EMAIL_BODY));
			cnt_ee_d += strlen(EMAIL_BODY);
			ee_i_d[0] = strlen(EMAIL_BODY);
			EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
			cnt_ee_i++;
			/* 8 */
			EESaveString(cnt_ee_d, EMAIL_DEST, strlen(EMAIL_DEST));
			cnt_ee_d += strlen(EMAIL_DEST);
			ee_i_d[0] = strlen(EMAIL_DEST);
			EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
			cnt_ee_i++;
			/* 9 */
			if(strlen(start_string)!=0)
			{
				EESaveString(cnt_ee_d, start_string, strlen(start_string));
				cnt_ee_d += strlen(start_string);
				ee_i_d[0] = strlen(start_string);
				EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
				cnt_ee_i++;
				/* 10 */
				EESaveString(cnt_ee_d, stop_string, strlen(stop_string));
				cnt_ee_d += strlen(stop_string);
				ee_i_d[0] = strlen(stop_string);
				EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
				cnt_ee_i++;
				/* 11 */
				EESaveString(cnt_ee_d, GMT, strlen(GMT));
				cnt_ee_d += strlen(GMT);
				ee_i_d[0] = strlen(GMT);
				EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
				cnt_ee_i++;
			
				cnt_ee_i = 0;
				cnt_ee_d = 11;
			
				GMT_hour_adding = atoi(GMT);
				
				dbg_string[0]=start_string[0];
				dbg_string[1]=start_string[1];
				dbg_string[2]='\0';
				start_alarm_time.tm_hour=atoi(dbg_string);
				
				dbg_string[0]=start_string[3];
				dbg_string[1]=start_string[4];			
				start_alarm_time.tm_min=atoi(dbg_string);
				
				dbg_string[0]=stop_string[0];
				dbg_string[1]=stop_string[1];
				stop_alarm_time.tm_hour=atoi(dbg_string);
				
				dbg_string[0]=stop_string[3];
				dbg_string[1]=stop_string[4];
				stop_alarm_time.tm_min=atoi(dbg_string);
			
				while(epoch<epochtime)
				{
					vTaskDelay(50);
					//Remember to enable the SNTP Client in the wizard.
					epoch=SNTPGetUTCSeconds();
				}
				now=(time_t)epoch;
				ts=localtime(&now);
				vTaskDelay(20);
				ts->tm_hour = (ts->tm_hour + GMT_hour_adding);
				// Correct if overflowed hour 0-24 format
				if(ts->tm_hour > 24)
					ts->tm_hour = ts->tm_hour - 24;
				else if(ts->tm_hour < 0)
					ts->tm_hour = ts->tm_hour +24;
				sprintf (dbg_string, "\nReceived date/time is: %s \r\n", asctime (ts) );
				_dbgwrite(dbg_string);
				RTCCSet(ts);
				vTaskDelay(50);
				RTCCAlarmConf(&start_alarm_time,REPEAT_INFINITE,EVERY_DAY,start_alarm);
				RTCCAlarmSet(ON);
				IOPut(LED1_Pin, 1);
				ParamSet = FALSE;
				
				WFSleep();
			}
			else
			{
				ee_i_d[0]=0;
				EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
				cnt_ee_i++;
				EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
				cnt_ee_i++;
				EESaveData(cnt_ee_i, ee_i_d, 1, EE_BYTE);
				cnt_ee_i++;
				timer=1;
				cnt_ee_i = 0;
				cnt_ee_d = 11;
				IOPut(LED1_Pin, 1);
				ParamSet = FALSE;
				
				WFHibernate();
			}	
		}
		
		vTaskDelay(25);
		
		//the main routine works only if the network-type is infrastructure and if the flyport is connected 
		if(/*AppConfig.networkType==WF_INFRASTRUCTURE&&WFStatus == CONNECTED&&*/timer==1)
		{
			PWMDuty(50,1);
			while(1)
			{
				anVal = get(light_sensor);
				/*sprintf(dbg_string,"--%2.1f--",(double)anVal);
				_dbgwrite(dbg_string);*/
				if(anVal>anVal_zero)
				{
					i++;
					if(i==COUNT_250MS)
						break;
					else if(i>COUNT_250MS)
					{
						i=COUNT_250MS+1;
						break;
					}
					vTaskDelay(25);
				}
				else
				{
					i=0;
					break;
				}
			}
			
			//time control and if this is true, the flyport sends an email
			if(i==COUNT_250MS)
			{
				if(WFStatus != CONNECTED)
				{
					WFOn();
					vTaskDelay(150);
					// Connection to Network
					if (WFCustomExist())
					{
						WFCustomLoad();
						UARTWrite(1,"Connecting to custom...\r\n");
						WFConnect(WF_CUSTOM);
					}
					while (WFStatus != CONNECTED);
					UARTWrite(1,"Flyport connected... hello world!\r\n");
					vTaskDelay(150);
					report = 5555;
					while(report!=0)
					{
						if(SMTPStart())
						{
							_dbgwrite("SMTP Started!\r\n");
				 
							//SERVER parameters
							SMTPSetServer(SERVER_NAME, MY_SMTP);
							SMTPSetServer(SERVER_USER, MY_EMAIL_USER);
							SMTPSetServer(SERVER_PASS, MY_EMAIL_PASS);
							SMTPSetServer(SERVER_PORT, MY_SMTP_PORT);
				 
							//Message parameters
							SMTPSetMsg(MSG_TO, EMAIL_DEST);
							SMTPSetMsg(MSG_BODY, EMAIL_BODY);
							SMTPSetMsg(MSG_FROM, MY_EMAIL);
							SMTPSetMsg(MSG_SUBJECT, EMAIL_SUBJECT);		 
				 
							_dbgwrite("Client SMTP initialized!\r\n");					  
				 
							//Sending email and waiting for report
							if(SMTPSend())
							{
								_dbgwrite("sending email...");
				 
								while(SMTPBusy() == TRUE)
								{
									_dbgwrite(".");
									vTaskDelay(30);
								}
								_dbgwrite("Email sent!\r\n");
							}
							else
								_dbgwrite("ERROR\r\n");
				 
							report = SMTPReport();
							sprintf(dbg_string, "Report result: %u\r\n", report);
							_dbgwrite(dbg_string);
							vTaskDelay(300);
						}
					}
					WFHibernate();
				}
			}
		}
	}
}
