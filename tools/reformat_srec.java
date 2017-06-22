import java.io.*;
import java.util.*;
import java.lang.StringBuilder;

public class reformat_srec{

  public static void main(String[] args) {
    new reformat_srec().start(args[0]);



  }
  public reformat_srec(){

  }
  public String content = "";
  public void start(String filename){
    try{
      long wordsCount = 0;
     wordsCount = LoadSrec(filename);
     System.out.println("words loaded "+wordsCount);
     String newcontent = "";
     int byteCount = 0;
     int checksum = 0;
     String wordsCountdata = Long.toHexString(wordsCount).toUpperCase();

     //System.out.println(wordsCountdata);
     wordsCountdata = AddZeroToFrontToFormPairsMin4(wordsCountdata);

     byteCount = wordsCountdata.length() /2 +1;

     checksum = byteCount + (int)Long.parseLong(wordsCountdata,16);
     if (checksum > 255) {

       checksum = ~checksum;
       checksum = checksum & 0xff;
       //System.out.println(AddZeroToFrontToFormPairs(checksum));
     }
     newcontent += ("S6" + AddZeroToFrontToFormPairs(byteCount) + wordsCountdata + AddZeroToFrontToFormPairs(checksum)) + "\n";

     //System.out.print(newcontent);
     //System.out.print(content);

     File rewritefile = new File(filename);
     FileOutputStream fos = new FileOutputStream(rewritefile,false);
     byte[] mybytes1 = newcontent.getBytes();
     fos.write(mybytes1);
     byte[] mybytes2 = content.getBytes();
     fos.write(mybytes2);
     fos.close();


   }catch (Exception e){
     e.printStackTrace();
   }
  }
  public String AddZeroToFrontToFormPairs(long i){
    String s = Long.toHexString(i).toUpperCase();
    if (s.length() % 2 != 0) {
      s = new StringBuilder(s).insert(0,"0").toString();
    }
    return s;
  }
  public String AddZeroToFrontToFormPairsMin4(String s){

    if (s.length() < 4) {
      StringBuilder sb = new StringBuilder(s);
      for (int i=0;i< 4 - s.length() ;i++ ) {
        sb.insert(0,"0");
      }
      s = sb.toString();
    }

    if (s.length() % 2 != 0) {
      s = new StringBuilder(s).insert(0,"0").toString();
    }
    //convert it to hex string
    return s;
  }

  public long LoadSrec(String name) throws Exception
  {
          FileReader in = new FileReader(name);
          BufferedReader br = new BufferedReader(in);
            long wordsLoaded = 0;
            String line = "";
            long linesCount = 0;
            StringBuilder sb = new StringBuilder();
            //Note: All hex values are big endian.

            //Read records
            while ((line = br.readLine()) != null)
            {
                sb.append(line + "\n");
                char linec[] = line.toCharArray();
                linesCount++;
                int index = 0;
                int checksum = 0;

                //Start code, always 'S'
                if (linec[index++] != 'S')
                    throw new Exception("Expecting 'S'");

                //Record type, 1 digit, 0-9, defining the data field
                //0: Vendor-specific data
                //1: 16-bit data sequence
                //2: 24 bit data sequence
                //3: 32-bit data sequence
                //5: Count of data sequences in the file. Not required.
                //7: Starting address for the program, 32 bit address
                //8: Starting address for the program, 24 bit address
                //9: Starting address for the program, 16 bit address
                int recordType = Integer.parseInt(String.valueOf(linec[index++]), 16);
                int addressLength = 0;
                switch (recordType)
                {
                    case 0:
                    case 1:
                    case 9:
                        addressLength = 2;
                        break;

                    case 2:
                    case 8:
                        addressLength = 3;
                        break;
                    case 5:
                    case 6:
                        throw new Exception("data already reformated");

                    case 3:
                    case 7:
                        addressLength = 4;
                        break;

                    default:
                        throw new Exception("Unknown record type");
                }

                //Byte count, 2 digits, number of bytes (2 hex digits) that follow (in address, data, checksum)
                int byteCount = Integer.parseInt(line.substring(index, index+2), 16);
                index += 2;
                checksum += byteCount;

                //Address, 4, 6 or 8 hex digits determined by the record type
                for (int i = 0; i < addressLength; i++)
                {
                    String ch = line.substring(index + i * 2, (index+ i * 2)+ 2);
                    checksum += Integer.parseInt(ch, 16);
                }

                int address = Integer.parseInt(line.substring(index, index+ addressLength * 2), 16);
                index += addressLength * 2;
                byteCount -= addressLength;
                //Data, a sequence of bytes.
                //java byte doesn't support unisnged byte, so we use int here
                int[] data = new int[byteCount - 1];
                for (int i = 0; i < data.length; i++)
                {
                    data[i] = Integer.decode("0x"+line.substring(index, index+ 2).toLowerCase());
                    //System.out.println(Integer.decode("0x"+line.substring(index, index+ 2).toLowerCase()));
                    index += 2;
                    checksum += data[i];
                }

                //Checksum, two hex digits. Inverted LSB of the sum of values, including byte count, address and all data.
                int readChecksum = (byte)Integer.parseInt(line.substring(index, index+ 2), 16);
                checksum = (~checksum & 0xFF);

                switch (recordType)
                {
                    case 3: //data intended to be stored in memory.
                        int localcount = 0;
                        for (int i = 0; i < data.length; i += 4)
                        {
                            int val = 0;
                            for (int j = i; j < i + 4; j++)
                            {
                                val <<= 8;
                                //System.out.println(data[j]);
                                //System.out.println(tempbyte);
                                val |= data[j];

                            }
                            localcount++;
                            val &= 0xffffffff;
                            //System.out.println(String.format("%08d",val));
                            //System.out.println(Integer.toHexString(val));
                        }

                        wordsLoaded += localcount;
                        break;

                    case 7: //entry point for the program.
                        // CPU.PC = (long)address;
                        System.out.println("pc "+address);
                        break;
                }
            }
            br.close();
            content = sb.toString();
            return wordsLoaded;
        }



}
