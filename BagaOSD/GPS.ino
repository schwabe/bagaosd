




/*GPS variables*/
int gpsFix=1;
float lat=0;
float lon=0;
float alt_MSL=0;
long iTOW=0;
float alt=0;
short hdop=9999;
short vdop=9999;
float speed_3d=0;
float ground_speed=0;
float heading=90;
float climb=0;
char data_update_event=0;
short time_year=0;
int time_month=0;
int time_day=0;
int time_hour=0;
int time_minute=0;
int time_second=0;
byte numsats=0;
byte ck_a=0;
byte ck_b=0;
byte GPS_step=0;
byte UBX_class=0;
byte UBX_id=0;
byte UBX_payload_length_hi=0;
byte UBX_payload_length_lo=0;
byte UBX_payload_counter=0;
byte UBX_buffer[128];
byte UBX_ck_a=0;
byte UBX_ck_b=0;

long origin_lat=0;
long origin_lon=0;
float origin_alt=0;

//long gps_utc_time_second=0;

void decode_gps(void)
{
	static unsigned long GPS_timer=0;
	byte data;
	int numc;

	numc = Serial.available();
	if (numc > 0){
		for (int i=0; i<numc; i++){ // We will loop through the serial buffer and store the incoming data into the array UBX_buffer[]
			data = Serial.read(); // On each loop of this for we are placing one byte at a time from the serial buffer
			//lets check to make sure we're not talking to the configuration program
			switch(GPS_step){ //Normally we start from zero. This is a state machine
			case 0:
				if(data==0xB5) // UBX sync char 1
					GPS_step++; //ooh! first data packet is correct. Jump to the next step.
				break;
			case 1:
				if(data==0x62) // UBX sync char 2
					GPS_step++; // The second data packet is correct, jump to step 2
				else GPS_step=0; //Nope, incorrect. Restart to step zero and try again.
				break;
			case 2:
				UBX_class = data; // This is the byte that will give our uBlox class
				checksum(UBX_class);
				GPS_step++;
				break;
			case 3:
				UBX_id = data; // This is the byte that will give our uBlox ID such as NAV-POSLLH, VELNED, STATUS, etc
				checksum(UBX_id);
				GPS_step++;
				break;
			case 4:
				UBX_payload_length_hi = data; // This is the first byte that will give us the payload length
				checksum(UBX_payload_length_hi);
				GPS_step++;
				break;
			case 5:
				UBX_payload_length_lo = data; // This is the second byte that will give us the payload length
				checksum(UBX_payload_length_lo);
				GPS_step++;
				break;
				// revised version thx to Christopher Barnes
			case 6: // Payload data read...
				// We need to process the data byte before we check the number of bytes so far
				if(UBX_payload_counter>127){
					GPS_step=8;
					break;
				}    //sometifvmes we get stuck here with bad data due to baudrate
				UBX_buffer[UBX_payload_counter] = data;
				checksum(data);
				UBX_payload_counter++;
				if (UBX_payload_counter < UBX_payload_length_hi) // We stay in this state until we reach the payload_length
				{
				}
				else
				{
					GPS_step++; // payload_length reached - next byte is checksum
				}
				break;
			case 7:
				UBX_ck_a=data;       // First checksum byte
				GPS_step++;
				break;
			case 8: // We end the GPS read...
				UBX_ck_b=data;       // Second checksum byte
				if((ck_a == UBX_ck_a) && (ck_b == UBX_ck_b)) // Verify the received checksum with the generated checksum
				{
					parse_ubx_gps(); // Parse the new GPS packet
					GPS_timer = millis(); //Restarting timer...
				}
				// Variable re-initialization
				GPS_step = 0;
				UBX_payload_counter = 0;
				ck_a = 0;
				ck_b = 0;
				break;
			} // End Switch
		} // End For
	} // End IF
} // End Function void decode_gps(void)
void parse_ubx_gps()
{
	if(UBX_id==0x02){ //ID NAV-POSLLH
		lon     = (float)join_4_bytes(&UBX_buffer[4])/10000000.0;
		lat     = (float)join_4_bytes(&UBX_buffer[8])/10000000.0;
        alt     = (float)join_4_bytes(&UBX_buffer[12])/1000.0;
        //float  current_alt = (float)join_4_bytes(&UBX_buffer[16])/1000.0;
        //alt_MSL = current_alt;
        alt_MSL = (float)join_4_bytes(&UBX_buffer[16])/1000.0;
		
		/*if(height_above_takeoff){
			alt     = alt - origin_alt;
			alt_MSL = alt_MSL - origin_alt;
                        if( origin_lat !=0 && abs(alt_MSL) > 3000 ) { //DMD pb sometimes alt_MSL badely set
                          origin_alt = current_alt;
                          alt_MSL = 0;
                        }
		}*/
	}
	
	if(UBX_id==0x03){ //ID NAV-STATUS
	    byte gps_flags = UBX_buffer[5];
	    if( gps_flags & 0x01 ) { //First bit to 1, then gpsFixOk (position and velocity valid and within DOP and ACC Masks,)
    		gpsFix=UBX_buffer[4];
    		if(gpsFix==3){		//locked
    		    if(origin_lat==0){
    		        origin_lat=lat;
    			origin_lon=lon;
    			origin_alt=alt_MSL;
    		    }
    		}
             }
        
            //DMD : get GPS time
            //if( gps_flags & 0x01000 ) { //Forth bit to 1, then Time of Week valid 
            //    utc_time_milli = (uint64_t)join_4_bytes(&UBX_buffer[0]);
    	    //}
	}

	if(UBX_id==0x06){
		numsats=UBX_buffer[47];
	}
	if (UBX_id==0x04) { //DOP
		hdop=(short)join_2_bytes(&UBX_buffer[12]);  //in CM?
		vdop=(short)join_2_bytes(&UBX_buffer[10]);  //in CM?
	}
	if(UBX_id==0x12){ // ID NAV-VELNED
	    climb         = (float)join_4_bytes(&UBX_buffer[12])/100.0; //DMD
		speed_3d      = (float)join_4_bytes(&UBX_buffer[16])/100.0;
		ground_speed  = (float)join_4_bytes(&UBX_buffer[20])/100.0;
		heading       = (float)join_4_bytes(&UBX_buffer[24])/100000.0;
	}
	if(UBX_id==0x21){ // ID NAV-TIMEUTC
	    byte time_flags = UBX_buffer[19]; //DMD
	    if( time_flags & 0x01 ) { //DMD : First bit : 1 = Valid Time of Week
    		time_month  = join_1_bytes(&UBX_buffer[14]);
    		time_day    = join_1_bytes(&UBX_buffer[15]);
    		time_year   = join_2_bytes(&UBX_buffer[12]);
    		time_hour   = join_1_bytes(&UBX_buffer[16]);
    		time_minute = join_1_bytes(&UBX_buffer[17]);
    		time_second = join_1_bytes(&UBX_buffer[18]);
    		
    		//gps_utc_time_second = time_second + time_minute * 60 + time_hour * 3600;
            }
        }
} // End Function void parse_ubx_gps()

void checksum(byte ubx_data){
	ck_a += ubx_data;
	ck_b += ck_a;
}

union long_union {
	int32_t dword;
	uint8_t  byte[4];
} longUnion;
union int_union {
	int16_t word;
	uint8_t  byte[2];
} intUnion;

int32_t join_4_bytes(byte Buffer[]){
	longUnion.byte[0] = *Buffer;
	longUnion.byte[1] = *(Buffer+1);
	longUnion.byte[2] = *(Buffer+2);
	longUnion.byte[3] = *(Buffer+3);
	return(longUnion.dword);}
int32_t join_2_bytes(byte Buffer[]){
	longUnion.byte[0] = *Buffer;
	longUnion.byte[1] = *(Buffer+1);
	return(longUnion.dword);}
int32_t join_1_bytes(byte Buffer[]){
	intUnion.byte[0] = *Buffer;
	return(intUnion.word);}
