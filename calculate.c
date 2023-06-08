#include "calculate.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

static const char bramA[] = "/dev/bramA"; // GRAD will be stored
static const char bramB[] = "/dev/bramB"; // QANGLE will be stored
static const char bramC[] = "/dev/bramC"; // BLOCKHIST will be stored - output
static const char bramD[] = "/dev/bramD"; // GRADOFS will be stored
static const char bramF[] = "/dev/bramF"; // GRADWEIGHTS will be stored
static const char bramG[] = "/dev/bramG"; // HISTOFS will be stored
static const char bramH[] = "/dev/bramH"; // HISTWEIGHTS will be stored
static const char registersIP[] = "/dev/GetBlockIP";

static const char gradFile[]      = "grad.txt";
static const char qangleFile[]    = "qangle.txt";
static const char blockHistFile[] = "blockHist.txt";
static const char gradOfs[]       = "gradOfs.txt";
static const char gradWeights[]   = "gradWeight.txt";
static const char histOfs[]       = "histOfs.txt";
static const char histWeights[]   = "histWeights.txt";

static const char receivedMsg[] = "RECEIVED";

static boolean b_writeToBram(const char* const inputFile, const char* const bramFileString, unsigned int length);
static boolean b_checkWriteOp(const char* const ptargetFile);
static boolean b_writeToIP(boolean start);
static boolean b_readResults(const char* const outputFile);
static boolean b_checkReadiness(boolean* readyFlag);

int main(int argc, char *argv[])
{
 
    boolean status = FALSE;
    boolean ready = FALSE;
    BlockInfo blockData;

    status = b_checkReadiness(&ready);
    
    

    if(status == TRUE)
    {
    	printf("\n[0]Device registers successfuly read.\n");
    }
    else
    {
    	printf("\n[0]Error occured during device registers read attempt.\n");
    }

    if(ready == TRUE)
    {
	

		/* GRAD array transfer to device*/
		status = b_writeToBram(gradFile, bramA, GRAD_QANGLE_SIZE);

		if(status == TRUE)
		{
			printf("\n[1]GRAD successfully transfered.\n");
		}
		else
		{
			printf("\n[1]GRAD transfer failed.\n");
		}


		/* QANGLE array transfer to device*/
		status = b_writeToBram(qangleFile, bramB, GRAD_QANGLE_SIZE);

		if(status == TRUE)
		{
			printf("\n[2]QANGLE successfully transfered.\n");
		}
		else
		{
			printf("\n[2]QANGLE transfer failed.\n");
		}


		/* GRADOFS array transfer to device*/
		status = b_writeToBram(gradOfs, bramD, GRADOFS_SIZE);

		if(status == TRUE)
		{
			printf("\n[3]GRADOFS successfully transfered.\n");
		}
		else
		{
			printf("\n[3]GRADOFS transfer failed.\n");
		}


		/* GRADWEIGHTS array transfer to device*/
		status = b_writeToBram(gradWeights, bramF, GRADWEIGHTS_SIZE);

		if(status == TRUE)
		{
			printf("\n[4]GRADWEIGHTS successfully transfered.\n");
		}
		else
		{
			printf("\n[4]GRADWEIGHTS transfer failed.\n");
		}


		/* HISTOFS array transfer to device*/
		status = b_writeToBram(histOfs, bramG, HISTOFS_SIZE);

		if(status == TRUE)
		{
			printf("\n[5]HISTOFS successfully transfered.\n");
		}
		else
		{
			printf("\n[5]HISTOFS transfer failed.\n");
		}


		/* HISTWEIGHTS array transfer to device*/
		status = b_writeToBram(histWeights, bramH, HISTWEIGHTS_SIZE);

		if(status == TRUE)
		{
			printf("\n[6]HISTWEIGHTS successfully transfered.\n");
		}
		else
		{
			printf("\n[6]HISTWEIGHTS transfer failed.\n");
		}


		for( int k = 0; k < NUMBER_OF_BLOCKS_PER_IMAGE; k++)
		{
			b_checkReadiness(&ready);
			if(ready == TRUE )
			{
				/* Request start*/
				status = b_writeToIP(TRUE);

				if(status == TRUE)
				{
					printf("\n[7]START successfully.\n");
				}
				else
				{
					printf("\n[7]START failed.\n");
				}

				b_checkReadiness(&ready);
				if(ready == TRUE )
				{
					status = b_readResults("blockHist.txt");

					if(status == TRUE)
					{
						printf("\nResults successfully written to blockHist.txt\n");
					}
					else
					{
						printf("\nWrite operation to blockHist.txt failed.\n");
					}
				}
				else
				{
					printf("\nDevice is not ready !!!!!!!!!!!!!!\n");
				}
			}
			else
			{
				printf("\nDevice is not ready\n");
			}
		}
		
		printf("\nAll 961 blocks successfully processed.\n");
		
		
    }
    else
    {
    	printf("\nDevice is not ready currently.\n");
    }

    
    return 0;
}

static boolean b_writeToBram(const char* const inputFile, const char* const bramFileString, unsigned int length)
{
	boolean returnValue = TRUE;
	IntToBytes tmp;


	FILE* p_inputFile = fopen(inputFile, READ_ACCESS);
	FILE* p_bramFile = fopen(bramFileString, WRITE_ACCESS);

	if(p_inputFile == NULL)
	{
		printf("\n[1]File opening unsuccessful.\n");
		returnValue = FALSE;
	}

	if(p_bramFile == NULL)
	{
		printf("\n[2]File opening unsuccessful.\n");
		returnValue = FALSE;
	}

	if(GRAD_QANGLE_SIZE == length )
	{
		for(int i = 0; i < BUFFERS_COUNT; i++)
		{
			for(int j = 0; j < WORDS_IN_BUFFER; j++)
			{
				fscanf(p_inputFile,"%d",&tmp.word);
				fprintf(p_bramFile,"%c%c%c%c",tmp.byte[3], tmp.byte[2], tmp.byte[1], tmp.byte[0]);
			}
			if( i != (BUFFERS_COUNT - 1) )
			{
				if(0 != fclose(p_bramFile))
				{
					printf("\n[3]File closing unsuccessful.\n");
					returnValue = FALSE;
				}
				else
				{
					while(!b_checkWriteOp(bramFileString));  
				}

				p_bramFile = fopen(bramFileString, WRITE_ACCESS);

				if(p_bramFile == NULL)
				{
					printf("\n[3]File opening unsuccessful.\n");
					returnValue = FALSE;
				}
			}
		}
	}
	else
	{
		for(int i = 0; i < length; i++)
		{
			fscanf(p_inputFile,"%d",&tmp.word);
			fprintf(p_bramFile,"%c%c%c%c",tmp.byte[3], tmp.byte[2], tmp.byte[1], tmp.byte[0]);
		}
	}

	if(0 != fclose(p_bramFile))
	{
		printf("\n[1]File closing unsuccessful.\n");
		returnValue = FALSE;
	}

	if(0 != fclose(p_inputFile))
	{
		printf("\n[2]File closing unsuccessful.\n");
		returnValue = FALSE;
	}

	return returnValue;
}

static boolean b_checkWriteOp(const char* const targetFile)
{
	char cmdMsg[COMMAND_MESSAGE_LENGTH]; //8u
	boolean status = FALSE;

	FILE* pFile = fopen(targetFile, READ_ACCESS);

	if(pFile != NULL)
	{
		fscanf(pFile,"%c%c%c%c%c%c%c%c",&cmdMsg[0],&cmdMsg[1],&cmdMsg[2],&cmdMsg[3],&cmdMsg[4],&cmdMsg[5],&cmdMsg[6],&cmdMsg[7]);

		if(!strncmp(receivedMsg, &cmdMsg[0], (unsigned long)COMMAND_MESSAGE_LENGTH)) 
		{
			status = TRUE;
		}
	}
	else
	{
		printf("\nNULL pointer in b_checkWriteOp function.\n");
	}

	fclose(pFile);

	return status;
}

static boolean b_writeToIP(boolean start)
{
	FILE* pFile;
	boolean status = FALSE;
	IntToBytes tmp;

	pFile = fopen(registersIP, WRITE_ACCESS);

	if(pFile != NULL)
	{
		
			tmp.word = (int)start;
			fprintf(pFile, "%c%c%c%c", tmp.byte[3], tmp.byte[2], tmp.byte[1], tmp.byte[0]);

			status = TRUE;
		
	}
	else
	{
		printf("\nFile opening failed - b_writeToIP.\n");
	}

	fclose(pFile);

	return status;
}

static boolean b_readResults(const char* const outputFile)
{
	IntToBytes tmp;
	boolean status = FALSE;

	FILE* pOutput = fopen(outputFile, WRITE); 
	FILE* pResultOutput = fopen(bramC, READ_ACCESS);

	if(pOutput != NULL)
	{
		if(pResultOutput != NULL)
		{
			
			for(int i = 0; i < BLOCKHIST_SIZE; i++)
			{
				fscanf(pResultOutput,"%c%c%c%c",&tmp.byte[3],&tmp.byte[2],&tmp.byte[1],&tmp.byte[0]);
				fprintf(pOutput,"%d\n",tmp.word);
				//printf("blockHist[%d]: %d\n", i, tmp.word);
			}
			status = TRUE;

		}
		else
		{
			printf("\n[2]NULL pointer in b_readResults function.\n");
		}
	}
	else
	{
		printf("\n[1]NULL pointer in b_readResults function.\n");
	}

	fclose(pOutput);
	fclose(pResultOutput);

	return status;
}

static boolean b_checkReadiness(boolean* readyFlag) 
{
	IntToBytes tmp;
	boolean status = 0;

	FILE* pFile = fopen(registersIP, READ_ACCESS);

	if(pFile != NULL)
	{
		
		fscanf(pFile, "%c%c%c%c", &tmp.byte[3], &tmp.byte[2], &tmp.byte[1], &tmp.byte[0]);
		if(tmp.word == 0)
		{
			*readyFlag = FALSE;
		}
		else
		{
			*readyFlag = TRUE;
		}

		status = TRUE;
		

	}
	else
	{
		printf("\n[1]NULL pointer in b_readIPregisters function.\n");
	}

	fclose(pFile);

	return status;
}
