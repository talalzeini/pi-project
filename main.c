// TALAL ZEINI CIST 0037 FINAL PROJECT
// TALAL ZEINI CIST 0037 FINAL PROJECT

#include <wiringPiI2C.h>
#include <string.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <mysql/mysql.h>
#define MAXTIMINGS 85
#define DHTPIN 7

// TALAL ZEINI CIST 0037 FINAL PROJECT

int dht11_dat[5] = { 0, 0, 0, 0, 0 };

// LCD ADDRESS IS 27 ACCORDING TO THE COMPUTER
// USED TERMINAL COMMANDS GIVEN BY THE INSTRUCTOR TO FIND THE ADDRESS
int LCDAddr = 0x27;
int BLEN = 1;
int fd;




void write(int x, int y, char data[]){
	int addr, i;
	int tmp;
	if (x < 0)  x = 0;
	if (x > 15) x = 15;
	if (y < 0)  y = 0;
	if (y > 1)  y = 1;

	// Move cursor
	addr = 0x80 + 0x40 * y + x;
	send_command(addr);

	tmp = strlen(data);
	for (i = 0; i < tmp; i++){
		send_data(data[i]);
	}
}





void read_dht11_dat()
{

    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;
    float f;
    dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;
    pinMode( DHTPIN, OUTPUT );
    digitalWrite( DHTPIN, LOW );
    delay( 18 );
    digitalWrite( DHTPIN, HIGH );
    delayMicroseconds( 40 );
    pinMode( DHTPIN, INPUT );
    for ( i = 0; i < MAXTIMINGS; i++ )
    {
        counter = 0;
        while ( digitalRead( DHTPIN ) == laststate )
        {
            counter++;
            delayMicroseconds( 1 );
            if ( counter == 255 )
            {
                break;
            }
        }
        laststate = digitalRead( DHTPIN );
        if ( counter == 255 )
            break;
        if ( (i >= 4) && (i % 2 == 0) )
        {
            dht11_dat[j / 8] <<= 1;
            if ( counter > 16 )
                dht11_dat[j / 8] |= 1;
            j++;
        }
    }
    if ( (j >= 40) &&
            (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
    {
        f = dht11_dat[2] * 9. / 5. + 32;


   int humidity[2] = { dht11_dat[0], dht11_dat[1]};
   char humidtyString[1024];
   sprintf(humidtyString, "Humidity = %d.%d", humidity[0], humidity[1]);

    int temperature[2] = {dht11_dat[2], dht11_dat[3]};
   char temperatureString[1024];
   sprintf(temperatureString, "Temp = %d.%d", temperature[0], temperature[1]);

        write(0,0, humidtyString);
        write(0,1, temperatureString);
         printf( "Humidity = %d.%d %% Temperature = %d.%d C (%.1f F)\n",
                dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );
    }
    else
    {
        printf( "Data not good, skip\n" );
    }
}
























void write_word(int data){
	int temp = data;
	if ( BLEN == 1 )
		temp |= 0x08;
	else
		temp &= 0xF7;
	wiringPiI2CWrite(fd, temp);
}

void send_command(int comm){
	int buf;
	// Send bit7-4 firstly
	buf = comm & 0xF0;
	buf |= 0x04;			// RS = 0, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);

	// Send bit3-0 secondly
	buf = (comm & 0x0F) << 4;
	buf |= 0x04;			// RS = 0, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);
}

void send_data(int data){
	int buf;
	// Send bit7-4 firstly
	buf = data & 0xF0;
	buf |= 0x05;			// RS = 1, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);

	// Send bit3-0 secondly
	buf = (data & 0x0F) << 4;
	buf |= 0x05;			// RS = 1, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);
}

void init(){
	send_command(0x33);	// Must initialize to 8-line mode at first
	delay(5);
	send_command(0x32);	// Then initialize to 4-line mode
	delay(5);
	send_command(0x28);	// 2 Lines & 5*7 dots
	delay(5);
	send_command(0x0C);	// Enable display without cursor
	delay(5);
	send_command(0x01);	// Clear Screen
	wiringPiI2CWrite(fd, 0x08);
}

void clear(){
	send_command(0x01);	//clear Screen
}
// TALAL ZEINI CIST 0037 FINAL PROJECT









void checkData(int day, char first[], char second[]){

    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    fd = wiringPiI2CSetup(LCDAddr);
	init();


    char *server = "localhost";
    char *user = "talal";
    char *password = "password";
    char *database = "finalproject";


    conn = mysql_init(NULL);

    /* Connect to database */
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /* send SQL query */
    if (mysql_query(conn, "show tables")){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    res = mysql_use_result(conn);

    /* output table name */
    printf("MySQL Tables in finalproject database:\n");

    while ((row = mysql_fetch_row(res)) != NULL)
        printf("%s \n", row[0]);

    char dayData[] = "20";
    dayData[0] = first;
    dayData[1] = second;
    char strDay[] = "20";
    strDay[0] = first;
    strDay[1] = second;

    char timeEntered[] = "16:56"; // by default
    printf("What time of May %d do you want to check the humidity and temperature?\n", day);
    printf("Enter a time in the format 00:00 in 24HOUR.\n");
    printf("For example, enter 16:56.\n");
    scanf("%s", &timeEntered);
    printf("You chose %s . Here's the data for this time.\n", timeEntered);



   char tempStr[50] = "select temp from finaldata where time = '";
   char timeToDisplay[] = "16:56";
    timeToDisplay[0]= timeEntered[0];
    timeToDisplay[1]= timeEntered[1];
    timeToDisplay[2]= timeEntered[2];
    timeToDisplay[3]= timeEntered[3];
    timeToDisplay[4]= timeEntered[4];
   strcat(tempStr, timeEntered);
   char qt[2] = "'";
   strcat(tempStr, qt);
   char dayStr[] = " and day = ";

   strcat(dayStr, dayData);
   strcat(tempStr, dayStr);
    printf("yes = %s", timeToDisplay);
    printf("%s", tempStr);
   printf("\n");

   if (mysql_query(conn, tempStr)){
      fprintf(stderr, "%s\n", mysql_error(conn));

    }

    res = mysql_use_result(conn);
    printf("Data in finaldata on May %d at time %s.\n", day, timeToDisplay);

    while ((row = mysql_fetch_row(res)) != NULL){
        char dateDisplay[] = "May ";
        strcat(dateDisplay, dayData);
        char coma[] = ", ";
        strcat(dateDisplay, coma);
        strcat(dateDisplay, timeToDisplay);
        write(0,0, dateDisplay);
        char t[] = "T=";
        strcat(t, row[0]);
        write(0,1,t);
        printf("\nTemperature : %s\n", t);
    }




    printf("%s", timeToDisplay);
    char humiStr[50] = "select humidity from finaldata where time = '";
    strcat(humiStr, timeToDisplay);
    char quote[2] = "'";
    strcat(humiStr, quote);
    strcat(humiStr, dayStr);
    printf("\nThis is where we run:\n%s", humiStr);


   if (mysql_query(conn, humiStr)){
      fprintf(stderr, "%s\n", mysql_error(conn));

    }
   res = mysql_use_result(conn);

    while ((row = mysql_fetch_row(res)) != NULL){
        char h[] = "H=";
        strcat(h, row[0]);
        write(8,1,h);
        printf("\nHumidity : %s\n", h);

    }

    mysql_free_result(res);
    mysql_close(conn);

}



void maxTemp(int day, char first[], char second[]){
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    fd = wiringPiI2CSetup(LCDAddr);
	init();
    char *server = "localhost";
    char *user = "talal";
    char *password = "password";
    char *database = "finalproject";
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    if (mysql_query(conn, "show tables")){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    res = mysql_use_result(conn);
    printf("Tables in finalproject database:\n");
    while ((row = mysql_fetch_row(res)) != NULL)
        printf("%s \n", row[0]);

    char dayData[] = "20";
    dayData[0] = first;
    dayData[1] = second;
    char maxTemp[50] = "select max(temp) from finaldata where day = ";
    strcat(maxTemp, dayData);
    printf("This is where we run:\n%s", maxTemp);
    printf("\n");
    if (mysql_query(conn, maxTemp)){
      fprintf(stderr, "%s\n", mysql_error(conn));
    }
    res = mysql_use_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL){
        char dateDisplay[] = "May ";
        strcat(dateDisplay, dayData);
        char coma[] = ", ";
        strcat(dateDisplay, coma);
        strcat(dateDisplay, " Max T");
        write(0,0, dateDisplay);
        // DISPLAYING MAX TEMPERATURE ON LCD DISPLAY
        printf("Max Temperature for May %d: T = %s\n", day,  row[0]);
        write(0,1, row[0]);
        printf("\nThe maximum temperature for that day is displayed on the LCD Screen.");
    }
    mysql_free_result(res);
    mysql_close(conn);
}





void minTemp(int day, char first[], char second[]){
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    fd = wiringPiI2CSetup(LCDAddr);
	init();
    char *server = "localhost";
    char *user = "talal";
    char *password = "password";
    char *database = "finalproject";
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    if (mysql_query(conn, "show tables")){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    res = mysql_use_result(conn);
    printf("Tables in finalproject database:\n");
    while ((row = mysql_fetch_row(res)) != NULL)
        printf("%s \n", row[0]);

    char dayData[] = "20";
    dayData[0] = first;
    dayData[1] = second;
    char minTemp[50] = "select min(temp) from finaldata where day = ";
    strcat(minTemp, dayData);
    printf("This is where we run:\n%s", minTemp);
    printf("\n");
    if (mysql_query(conn, minTemp)){
      fprintf(stderr, "%s\n", mysql_error(conn));
    }
    res = mysql_use_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL){
        char dateDisplay[] = "May ";
        strcat(dateDisplay, dayData);
        char coma[] = ", ";
        strcat(dateDisplay, coma);
        strcat(dateDisplay, " Min T");
        write(0,0, dateDisplay);
        // DISPLAYING MIN TEMPERATURE ON LCD DISPLAY
        printf("Min Temperature for May %d: T = %s\n", day,  row[0]);
        write(0,1, row[0]);
        printf("\nThe minimum temperature for that day is displayed on the LCD Screen.");
    }
    mysql_free_result(res);
    mysql_close(conn);
}









void maxHumidity(int day, char first[], char second[]){
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    fd = wiringPiI2CSetup(LCDAddr);
	init();
    char *server = "localhost";
    char *user = "talal";
    char *password = "password";
    char *database = "finalproject";
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    if (mysql_query(conn, "show tables")){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    res = mysql_use_result(conn);
    printf("Tables in finalproject database:\n");
    while ((row = mysql_fetch_row(res)) != NULL)
        printf("%s \n", row[0]);

    char dayData[] = "20";
    dayData[0] = first;
    dayData[1] = second;
    char maxHumidity[50] = "select max(humidity) from finaldata where day = ";
    strcat(maxHumidity, dayData);
    printf("This is where we run:\n%s", maxHumidity);
    printf("\n");
    if (mysql_query(conn, maxHumidity)){
      fprintf(stderr, "%s\n", mysql_error(conn));
    }
    res = mysql_use_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL){
        char dateDisplay[] = "May ";
        strcat(dateDisplay, dayData);
        char coma[] = ", ";
        strcat(dateDisplay, coma);
        strcat(dateDisplay, " Max H");
        write(0,0, dateDisplay);
        // DISPLAYING MAX HUMIDITY ON LCD DISPLAY
        printf("Max Humidity for May %d: T = %s\n", day,  row[0]);
        write(0,1, row[0]);
        printf("\nThe maximum humidity for that day is displayed on the LCD Screen.");
    }
    mysql_free_result(res);
    mysql_close(conn);
}


void minHumidity(int day, char first[], char second[]){
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    fd = wiringPiI2CSetup(LCDAddr);
	init();
    char *server = "localhost";
    char *user = "talal";
    char *password = "password";
    char *database = "finalproject";
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    if (mysql_query(conn, "show tables")){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    res = mysql_use_result(conn);
    printf("Tables in finalproject database:\n");
    while ((row = mysql_fetch_row(res)) != NULL)
        printf("%s \n", row[0]);

    char dayData[] = "20";
    dayData[0] = first;
    dayData[1] = second;
    char minHumidity[50] = "select min(humidity) from finaldata where day = ";
    strcat(minHumidity, dayData);
    printf("This is where we run:\n%s", minHumidity);
    printf("\n");
    if (mysql_query(conn, minHumidity)){
      fprintf(stderr, "%s\n", mysql_error(conn));
    }
    res = mysql_use_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL){
        char dateDisplay[] = "May ";
        strcat(dateDisplay, dayData);
        char coma[] = ", ";
        strcat(dateDisplay, coma);
        strcat(dateDisplay, " Min H");
        write(0,0, dateDisplay);
        // DISPLAYING MIN HUMIDITY ON LCD DISPLAY
        printf("Min Humidity for May %d: T = %s\n", day,  row[0]);
        write(0,1, row[0]);
        printf("\nThe minimum humidity for that day is displayed on the LCD Screen.");
    }
    mysql_free_result(res);
    mysql_close(conn);
}



void avgTemp(int day, char first[], char second[]){
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    fd = wiringPiI2CSetup(LCDAddr);
	init();
    char *server = "localhost";
    char *user = "talal";
    char *password = "password";
    char *database = "finalproject";
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    if (mysql_query(conn, "show tables")){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    res = mysql_use_result(conn);
    printf("Tables in finalproject database:\n");
    while ((row = mysql_fetch_row(res)) != NULL)
        printf("%s \n", row[0]);

    char dayData[] = "20";
    dayData[0] = first;
    dayData[1] = second;
    char avgTemp[50] = "select avg(temp) from finaldata where day = ";
    strcat(avgTemp, dayData);
    printf("This is where we run:\n%s", avgTemp);
    printf("\n");
    if (mysql_query(conn, avgTemp)){
      fprintf(stderr, "%s\n", mysql_error(conn));
    }
    res = mysql_use_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL){
        char dateDisplay[] = "May ";
        strcat(dateDisplay, dayData);
        char coma[] = ", ";
        strcat(dateDisplay, coma);
        strcat(dateDisplay, " Avg T");
        write(0,0, dateDisplay);
        // DISPLAYING AVERAGE TEMPERATURE ON LCD DISPLAY
        printf("Average Temperature for May %d: T = %s\n", day,  row[0]);
        write(0,1, row[0]);
        printf("\nThe average temperature for that day is displayed on the LCD Screen.");
    }
    mysql_free_result(res);
    mysql_close(conn);
}






void avgHumidity(int day, char first[], char second[]){
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    fd = wiringPiI2CSetup(LCDAddr);
	init();
    char *server = "localhost";
    char *user = "talal";
    char *password = "password";
    char *database = "finalproject";
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    if (mysql_query(conn, "show tables")){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    res = mysql_use_result(conn);
    printf("Tables in finalproject database:\n");
    while ((row = mysql_fetch_row(res)) != NULL)
        printf("%s \n", row[0]);

    char dayData[] = "20";
    dayData[0] = first;
    dayData[1] = second;
    char avgHumidity[50] = "select avg(humidity) from finaldata where day = ";
    strcat(avgHumidity, dayData);
    printf("This is where we run:\n%s", avgHumidity);
    printf("\n");
    if (mysql_query(conn, avgHumidity)){
      fprintf(stderr, "%s\n", mysql_error(conn));
    }
    res = mysql_use_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL){
        char dateDisplay[] = "May ";
        strcat(dateDisplay, dayData);
        char coma[] = ", ";
        strcat(dateDisplay, coma);
        strcat(dateDisplay, " Avg H");
        write(0,0, dateDisplay);
        // DISPLAYING AVERAGE Humidity ON LCD DISPLAY
        printf("Average Humidity for May %d: T = %s\n", day,  row[0]);
        write(0,1, row[0]);
        printf("\nThe average humidity for that day is displayed on the LCD Screen.");
    }
    mysql_free_result(res);
    mysql_close(conn);
}







void dayOptions(){
        printf("What do you want from that day?\n");
        printf("1.\tMaximum Temperature.\n");
        printf("2.\tMaximum Humidity.\n");
        printf("3.\tMinimum Temperature.\n");
        printf("4.\tMinimum Humidity.\n");
        printf("5.\tData about a specific time of that day.\n (Extra Credit)\n");
        printf("6.\tAverage Temperature.\n");
        printf("7.\tAverage Humidity.\n");

        printf("Enter a number for your choice: ");
}


int main(){

    time_t rawtime;
    struct tm * timeinfo;
    time( &rawtime );
    timeinfo = localtime( &rawtime );
    printf("Current Time = %02d:%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min);



    printf("Please select a day from the three dates available by entering 1, 2 or 3:\n");
    printf("1.\t 20 May\n");
    printf("2.\t 12 May\n");
    printf("3.\t 26 May\n\n");

    printf("4.\t OR Get current temperature and humidity.\n");
    int daySelected = 0;
    scanf("%d", &daySelected);
    printf("You selected option %d.\n", daySelected);
    if(daySelected == 1){
        printf("Which means you want data from May 20.\n");
        dayOptions();
        int choice = 0;
        scanf("%d", &choice);
        if(choice == 1){
            printf("\n\nMaximum Temp for May 20.\n");
            maxTemp(20, '2', '0');
        }
        else if(choice == 2){
            printf("\n\nMaximum Humidity for May 20.\n");
            maxHumidity(20, '2', '0');
        }
        else if(choice == 3){
            printf("\n\nMinimum Temp for May 20.\n");
            minTemp(20, '2', '0');
        }
        else if(choice == 4){
            printf("\n\nMinimum Humidity for May 20.\n");
            minHumidity(20, '2', '0');
        }
        else if(choice == 5){
            checkData(20, '2', '0');
        }else if(choice == 6){
            printf("\n\nAverage Temperature for May 20.\n");
            avgTemp(20, '2', '0');
        }else if(choice == 7){
            printf("\n\nAverage Humidity for May 20.\n");
            avgHumidity(20, '2', '0');
        }else{
            printf("\nInvalid Input. Thanks for trying.\n");
        return 0;
        }
    }

    else if(daySelected == 2){
        printf("Which means you want data from May 12.\n");
        dayOptions();
        int choice = 0;
        scanf("%d", &choice);
        if(choice == 1){
            printf("\n\nMaximum Temp for May 12.\n");
            maxTemp(12, '1', '2');
        }
        else if(choice == 2){
            printf("\n\nMaximum Humidity for May 12.\n");
            maxHumidity(12, '1', '2');
        }
        else if(choice == 3){
            printf("\n\nMinimum Temp for May 12.\n");
	        minTemp(12, '1', '2');
        }
        else if(choice == 4){
            printf("\n\nMinimum Humidity for May 12.\n");
		    minHumidity(12, '1', '2');
        }
        else if(choice == 5){
       	    checkData(12, '1', '2');
        }else if(choice == 6){
            printf("\n\nAverage Temperature for May 12.\n");
            avgTemp(12, '1', '2');
        }else if(choice == 7){
            printf("\n\nAverage Humidity for May 12.\n");
            avgHumidity(12, '1', '2');
        }else{
            printf("\nInvalid Input. Thanks for trying.\n");
        return 0;
        }
    }
    else if(daySelected == 3){
        printf("Which means you want data from May 26.\n");
        dayOptions();
        int choice = 0;
        scanf("%d", &choice);
        if(choice == 1){
            printf("\n\nMaximum Temp for May 26.\n");
            maxTemp(26, '2', '6');
        }
        else if(choice == 2){
            printf("\n\nMaximum Humidity for May 26.\n");
            maxHumidity(26, '2', '6');
        }
        else if(choice == 3){
            printf("\n\nMinimum Temp for May 26.\n");
            minTemp(26, '2', '6');
        }
        else if(choice == 4){
            printf("\n\nMinimum Humidity for May 26.\n");
            minHumidity(26, '2', '6');
        }else if(choice == 5){
            checkData(26, '2', '6');
        }else if(choice == 6){
            printf("\n\nAverage Temperature for May 26.\n");
            avgTemp(26, '2', '6');
        }else if(choice == 7){
            printf("\n\nAverage Humidity for May 26.\n");
            avgHumidity(26, '2', '6');
        }else{
            printf("\nInvalid Input. Thanks for trying.\n");
        return 0;
        }
    }else if (daySelected  == 4){
fd = wiringPiI2CSetup(LCDAddr);
	init();
 if ( wiringPiSetup() == -1 )
        exit( 1 );
    while ( 1 )
    {
        read_dht11_dat();
        delay( 1000 );
    }

    }else{
        printf("Invalid Input. By Default we'll choose 20 May.\n");
        dayOptions();
        int choice = 0;
        scanf("%d", &choice);
        if(choice == 1){
            printf("\n\nMaximum Temp for May 20.\n");
            maxTemp(20, '2', '0');
        }
        else if(choice == 2){
            printf("\n\nMaximum Humidity for May 20.\n");
            maxHumidity(20, '2', '0');
        }
        else if(choice == 3){
            printf("\n\nMinimum Temp for May 20.\n");
            minTemp(20, '2', '0');
        }
        else if(choice == 4){
            printf("\n\nMinimum Humidity for May 20.\n");
            minHumidity(20, '2', '0');
        }
        else if(choice == 5){
            checkData(20, '2', '0');
        }else if(choice == 6){
            printf("\n\nAverage Temperature for May 20.\n");
            avgTemp(20, '2', '0');
        }else if(choice == 7){
            printf("\n\nAverage Humidity for May 20.\n");
            avgHumidity(20, '2', '0');
        }else{
            printf("\nInvalid Input. Thanks for trying.\n");
        return 0;
        }
    }









    //if (mysql_query(conn, "update finaldata set temp = 12.3 where time = '16:11'")){
      //  fprintf(stderr, "%s\n", mysql_error(conn));
      //  exit(1);
   // }

    //if (mysql_query(conn, "insert into finaldata values (26, '15:09', 27.0, 55.3)")){
      //  fprintf(stderr, "%s\n", mysql_error(conn));
       // exit(1);
   // }



    if ( wiringPiSetup() == -1 )
        exit( 1 );
    while ( 1 )
    {
     //  read_dht11_dat();
    delay( 1000 );
    }
    return(0);

	// for ( int x = 0; x < 2; x++){
	// 	for(int y = 0; y < 16; y++){
	// 		clear();
	// 		write(y, x, ".");
	// 		delay(2000);
	// 	}
	// }
}



// TALAL ZEINI CIST 0037 FINAL PROJECT


// TALAL ZEINI CIST 0037 FINAL PROJECT

