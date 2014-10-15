
#include <stdafx.h>
#include <stdio.h>
#include<conio.h>
#include<stdlib.h>
extern "C" {
	#include <FreeRTOS.h>
	#include <task.h>
	#include <semphr.h> 
}
#include <interface.h>





xSemaphoreHandle Semkit = NULL;
xSemaphoreHandle Sem_callibrarion = NULL;


xSemaphoreHandle Sem_x = NULL;
xSemaphoreHandle Sem_z = NULL;

xQueueHandle	  mbx_x    = NULL;
xQueueHandle	  mbx_z    = NULL;



uInt8 read_port(int porto)
{
	uInt8 aa =0;
	xSemaphoreTake( Semkit, portMAX_DELAY );		 
	aa = ReadDigitalU8(porto);
	xSemaphoreGive( Semkit);
	return(aa);
}

void write_port(int porto, uInt8 value)
{
	uInt8 aa =0;
	xSemaphoreTake( Semkit, portMAX_DELAY );		 
	WriteDigitalU8(porto, value);
	xSemaphoreGive( Semkit);
}



bool getBit(uInt8 variable, uInt8 n_bit)
{
	int result = variable & (1<<n_bit);
	return( result!=0  ); // a value not zero is TRUE
}



void setBit(uInt8  &variable, int bit, bool value)
{
	uInt8  mask_on = (uInt8)(1<<bit);
	uInt8  mask_off = (uInt8)0xff - mask_on; 
	if(value)  variable |=    mask_on;
	else         variable &=    mask_off;
}



int actual_x()
{
	int p;
	int position =-1;

	p=read_port(0);
	if(getBit(p,2)==0)
		position=1;
	else if(getBit(p,1)==0)
		position=2;
	else if(getBit(p,0)==0)
		position=3;
	return(position);
}


bool is_at_x(int  x)
{
	if(actual_x()==x)
		return true;
	else return false;
}


int actual_y()
{
	int p;
	int position =-1;

	p=read_port(0);
	if(getBit(p,5)==0)
		position=1;
	else if(getBit(p,4)==0)
		position=2;
	else if(getBit(p,3)==0)
		position=3;
	return(position);
}


bool is_at_y(int  y)
{
	if(actual_y()==y)
		return true;
	else return false;
}





int actual_z()
{
	int p1,p0;
	int position =-1;

	p0=read_port(0);
	p1=read_port(1);
	if(getBit(p1,3)==0)
		position=1;
	else if(getBit(p1,1)==0)
		position=2;
	else if(getBit(p0,7)==0)
		position=3;
	return(position);
}


bool is_at_z(int  z)
{
	if(actual_z()==z)
		return true;
	else return false;
}


void move_x_left()
{
	uInt8 p  = read_port(2); //  ler conteúdo do porto 2 para var. p
	setBit(p, 7,0); 
	setBit(p, 6,1);     //  colocar bit 6 de variavel p com valor 1
	write_port(2,p); //  actualizar porto 2 com novo valor de var. p}
}


void move_x_right()
{
	uInt8 p  = read_port(2); //  ler conteúdo do porto 2 para var. p
	setBit(p, 7,1); 
	setBit(p, 6,0);     //  colocar bit 7 de variavel p com valor 1
	write_port(2,p); //  actualizar porto 2 com novo valor de var. p}
}

void move_y_inside()
{
	uInt8 p  = read_port(2); 
	setBit(p, 5,1); 
	setBit(p, 4,0);     
	write_port(2,p); 
}

void move_y_outside()
{
	uInt8 p  = read_port(2); 
	setBit(p, 5,0); 
	setBit(p, 4,1);     
	write_port(2,p); 
}


void move_z_up()
{
	uInt8 p  = read_port(2); 
	setBit(p, 3,1); 
	setBit(p, 2,0);     
	write_port(2,p); 
}

void move_z_down()
{
	uInt8 p  = read_port(2); 
	setBit(p, 3,0); 
	setBit(p, 2,1);     
	write_port(2,p); 
}



void stop_x()
{
	uInt8 p  = read_port(2); //  ler conteúdo do porto 2 para var. p
	setBit(p, 7,0); 
	setBit(p, 6,0);     //  colocar bit 6 de variavel p com valor 1
	write_port(2,p); //  actualizar porto 2 com novo valor de var. p}
}


void stop_y()
{
	uInt8 p  = read_port(2); //  ler conteúdo do porto 2 para var. p
	setBit(p, 5,0); 
	setBit(p, 4,0);     //  colocar bit 6 de variavel p com valor 1
	write_port(2,p); //  actualizar porto 2 com novo valor de var. p}
}

void stop_z()
{
	uInt8 p  = read_port(2); //  ler conteúdo do porto 2 para var. p
	setBit(p, 3,0); 
	setBit(p, 2,0);     //  colocar bit 6 de variavel p com valor 1
	write_port(2,p); //  actualizar porto 2 com novo valor de var. p}
}





void goto_x(int x)
{
	int x_pos= actual_x();
	if (x_pos< x)
		move_x_right();
	else if (x_pos > x)
		move_x_left(); 
	while( !is_at_x(x) )  
	{ 
		vTaskDelay(10);
	}   //    while position not reached
	stop_x();   // já chegou.
}

void goto_y(int y)
{
	int y_pos= actual_y();
	if (y_pos<y)
		move_y_inside();
	else  if (y_pos>y)
		move_y_outside(); 
	while( !is_at_y(y) )  
	{ 
		vTaskDelay(10);
	}   //    while position not reached
	stop_y();   // já chegou.
}
void goto_z(int z)
{
	int z_pos= actual_z();
	if (z_pos<z)
		move_z_up();
	else  if (z_pos>z)
		move_z_down(); 
	while( !is_at_z(z) )  
	{ 
		vTaskDelay(10);
	}   //    while position not reached
	stop_z();   // já chegou.
}


bool is_at_z_up()
{
	uInt8 p1  = read_port(1);
	uInt8 p0  = read_port(0);

	if( !getBit(p1,2) || !getBit(p1,0)  || !getBit(p0,6) )
		return(true);
	return(false);
}

bool is_at_z_dn()
{
	uInt8 p1  = read_port(1);
	uInt8 p0  = read_port(0);

	if( !getBit(p1,3) || !getBit(p1,1)  || !getBit(p0,7) )
		return(true);
	return(false);
}



void goto_z_up()
{
	move_z_up();
	while( !is_at_z_up()  )
		vTaskDelay(10);
	stop_z();
}


void goto_z_dn()
{
	move_z_down();
	while( ! is_at_z_dn()  )
		vTaskDelay(10);
	stop_z();
}



void put_in_cell()
{
	goto_z_up();
	//vTaskDelay(10);
	goto_y(3);
	//Sleep(10);
	goto_z_dn();
	//Sleep(10);
	goto_y(2);
}

void goto_xz(int x,int z)
{
	//goto_x(x);
	//goto_z(z);
	xQueueSend(mbx_x, &x , 0);		
	xQueueSend(mbx_z, &z , 0);		
}

void auto_callibration(void*)
{
	printf("Callibrating");

	int key=0;

	//printf("\nd");
	move_x_right(); //d
	while( ! (is_at_x(1)  || is_at_x(2) || is_at_x(3) ) )
		vTaskDelay(10);
	stop_x();
	//printf("\nd");

	//printf("\na");
	move_x_left();//a
	while( ! (is_at_x(1)  || is_at_x(2) || is_at_x(3) ) )
		vTaskDelay(10);
	stop_x();
	//printf("\na");

	//printf("\nw");
	move_z_up();//w
	while( ! (is_at_z(1)  || is_at_z(2) || is_at_z(3) ) )
		vTaskDelay(10);
	stop_z();
	//printf("\nw");

	//printf("\nz");
	move_z_down(); //z
	while( ! (is_at_z(1)  || is_at_z(2) || is_at_z(3) ) )
		vTaskDelay(10);
	stop_z();
	//printf("\nz");

	/*printf("\ne");
	move_y_inside();//e
	while( ! ( is_at_y(2) || is_at_y(3) ) )
		vTaskDelay(10);
	stop_y();
	printf("\ne");*/

	printf("\nr");
	move_y_outside();//r
	while( ! (is_at_y(1)  || is_at_y(2) ) )
		vTaskDelay(10);
	stop_y();
	printf("\nr");

	write_port(2,0);
	printf("Leaving callibration\n");

	xSemaphoreGive( Sem_callibrarion);

}

void callibration(void *)
{
	int key = 0;

	printf("Entering manual callibration.\n");
	printf("Press w,z,a,d,e,r\n");
	while(key!=27 )  // terminate with ESCAPE key
	{
		key = _getch();
		if(key==27)
		{
			stop_x();
			stop_y();
			stop_z();
		}
		if(key=='d' || key=='D')
		{
			move_x_right();
			while( ! (is_at_x(1)  || is_at_x(2) || is_at_x(3) ) )
				vTaskDelay(10);
			stop_x();
		}
		if(key=='a' || key=='A')
		{
			move_x_left();
			while( ! (is_at_x(1)  || is_at_x(2) || is_at_x(3) ) )
				vTaskDelay(10);
			stop_x();
		}

		if(key=='w' || key=='W')
		{
			move_z_up();
			while( ! (is_at_z(1)  || is_at_z(2) || is_at_z(3) ) )
				vTaskDelay(10);
			stop_z();
		}
		if(key=='z' || key=='Z')
		{
			move_z_down();
			while( ! (is_at_z(1)  || is_at_z(2) || is_at_z(3) ) )
				vTaskDelay(10);
			stop_z();
		}

		if(key=='e' || key=='e')
		{
			move_y_inside();
			while( ! ( is_at_y(2) || is_at_y(3) ) )
				vTaskDelay(10);
			stop_y();
		}
		if(key=='r' || key=='R')
		{
			move_y_outside();
			while( ! (is_at_y(1)  || is_at_y(2) ) )
				vTaskDelay(10);
			stop_y();
		}
	}   
	write_port(2,0);
	printf("Leaving callibration\n");
	xSemaphoreGive( Sem_callibrarion);

}







void vTaskHorizontal( void * pvParameters )
 {	 
     while(TRUE)
     {		
		//go right
		    uInt8 aa =read_port(2);
		write_port( 2, (aa & (0xff-0x40)) | 0x80);
	
		// wait till last sensor
		while(  (read_port(0) & 0x01)  ){vTaskDelay( 0);}
		
		// go left		
		aa =read_port(2);
		write_port( 2, (aa & (0xff-0x80)) | 0x40);
		
		// wait till last sensor
		while( (read_port(0) & 0x04) )	{vTaskDelay(0);}
		
     }
 }


void vTaskVertical( void * pvParameters )
 {	 
     while(TRUE)
     {		
		//go up
	    uInt8 aa =read_port(2);
		write_port( 2, (aa & (0xff-0x04)) | 0x08);		
	
		// wait till last sensor		
		while(  (read_port(0) & 0x40)  ){vTaskDelay( 0);}
		
		// go left		
		aa =read_port(2);
		write_port( 2, (aa & (0xff-0x08)) | 0x04);
	
		// wait till last sensor		
		while( (read_port(1) & 0x08) )	{vTaskDelay( 0);}
     }
 } 

void vUser_interface(void *)
{
	printf("\nvUser_interface: waiting for callibration...");
	xSemaphoreTake( Sem_callibrarion, portMAX_DELAY );		 
	printf("\nvUser_interface: entering operation...");
	while(true)
	{
		printf("\nenter option=");
		int op = _getch();
		op = op -'0';
		printf("\n%d selected...", op);
		if(op==1)
		{
			printf("\nColocar aqui o codigo para colocar um produto numa celula..");
			int x=0;
			printf("\nenter x="); 
			x=_getch();
			putchar(x);
			x=x-'0';
			
			int z=0;
			printf("\nenter z="); 			
			z=_getch();
			putchar(z);
			z=z-'0';
			if(x>=1 && x<=3 && z>=1 && z<=3)
			{
				goto_xz(x,z);
			}
		}
		if(op==2)
		{
			put_in_cell();
		}
	}
}

void goto_x_task(void *)
{
	while(true)
	{
		int x;
		xQueueReceive( mbx_x, &x , portMAX_DELAY );
		goto_x(x);

	}
}


void goto_z_task(void *)
{
	while(true)
	{
		int z;
		xQueueReceive( mbx_z, &z , portMAX_DELAY );
		goto_z(z);
	}

}


void main(void)
{
	create_DI_channel(0);
	create_DI_channel(1);
	create_DO_channel(2);

	//callibration();

	Semkit = xSemaphoreCreateCounting(1, 1);
	Sem_callibrarion = xSemaphoreCreateCounting(1, 0);

	Sem_x = xSemaphoreCreateCounting(1, 0);
	Sem_z = xSemaphoreCreateCounting(1, 0);

	mbx_x    = xQueueCreate( /*size of queue*/ 10, /*size of message*/ sizeof( int) );
	mbx_z    = xQueueCreate( /*size of queue*/ 10, /*size of message*/ sizeof( int) );


	// 1st task, callibration
	xTaskCreate(auto_callibration,(const signed char *const) 	"ErrorChk2", 100, NULL, 0, NULL );
	
	// remaining tasks, operation
	//xTaskCreate(vTaskHorizontal,(const signed char *const) 	"ErrorChk1", 100, NULL, 0, NULL );
	//xTaskCreate(vTaskVertical,(const signed char *const) 	"ErrorChk2", 100, NULL, 0, NULL );
	xTaskCreate(vUser_interface,(const signed char *const) 	"ErrorChk2", 100, NULL, 0, NULL );
	
	xTaskCreate(goto_x_task,(const signed char *const) 	"ErrorChk2", 100, NULL, 0, NULL );
	xTaskCreate(goto_z_task,(const signed char *const) 	"ErrorChk2", 100, NULL, 0, NULL );

	vTaskStartScheduler();
	//write_port( 2, 0);
}

