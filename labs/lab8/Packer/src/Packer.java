import java.io.File;
import java.io.RandomAccessFile;


public class Packer {

	public static void main(String[] args) throws Exception {
		RandomAccessFile f = new RandomAccessFile(new File("/Users/benjholla/Desktop/Antivirus demo/CVE_2012_4681/bin/techniques/d/Application.class"), "r");
		byte[] b = new byte[(int)f.length()];
		f.read(b);
		f.close();
		
		byte key = (byte) 0xF0;
		for(int i=0; i<b.length; i++){
			b[i] = (byte) (b[i] ^ key);
		}
		
		System.out.println(Base64.encode(b));
	}

}
