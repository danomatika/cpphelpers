
#include <oscframework/oscframework.h>
#include <iostream>

using namespace std;
using namespace osc;

void testTimeTag();
void testSender();

class TestReceiver : public OscReceiver
{
	public:
		TestReceiver() : OscReceiver(), bDone(false) {}
        
        void poll()
        {
        	bDone = false;
            
        	// poll for messages until "/quit" is received
        	while(!bDone)
            {
            	int num = handleMessages(0);
            	if(num > 0)
                {
                	cout << "TestReceiver: received " << num << " bytes" << endl;
                }
                else
                {
                	cout << "TestReceiver: still waiting ..." << endl;    
                }
            }
        }
        
        bool bDone;
        
    protected:
    	bool process(const ReceivedMessage& message, const MessageSource& source)
        {
        	cout << "TestReceiver: received message " << message.path() << " " << message.types() << endl;

			if(message.checkPathAndTypes("/test3", "TFcNIihfdsSmtb"))
            {
            	cout << "/test3 parsing all mesage types" << " " << message.typeTag(0) << endl
            		 << " bool T: " << message.asBool(0) << endl
               		 << " bool F: " << message.asBool(1) << endl
                     << " char: '" << message.asChar(2) << "'" << endl
                     << " nil" << endl			// message arg 3
                     << " infinitum" << endl	// message arg 4
                     << " int32: " << message.asInt32(5) << endl
                     << " int64: " << message.asInt64(6) << endl
                     << " float: " << message.asFloat(7) << endl
                     << " double: " << message.asDouble(8) << endl
            		 << " string: \"" << message.asString(9) << '"' << endl
                     << " symbol: \"" << message.asSymbol(10) << '"' << endl
                     << " midi: " << hex << message.asMidiMessage(11) << dec << endl
                     << " timetag: " << message.asTimeTag(12).sec << " " << message.asTimeTag(12).frac << endl
                     << " blob: \"" << std::string((char*) message.asBlob(13).data) << '"' << endl;
            
            	return true;
            }
            
            for(unsigned int i = 0; i < message.numArgs(); ++i)
            {
            	cout << "arg " << i << " '" << message.typeTag(i) << "' ";
                message.printArg(i);
            	cout << endl;
            }
            
            if(message.path() == "/quit")
            {
            	bDone = true;
            }
 
        	return true;
        }
};

int main(int argc, char *argv[])
{
    cout << "starting oscframework test" << endl;

	cout << "testing timetag" << endl;
    testTimeTag();
    cout << "done" << endl << endl;

	TestReceiver receiver;
    
    receiver.setup(9990);
    
    sleep(2);
    
    cout << "running receiver without thread" << endl;
	testSender();
    receiver.poll();
	cout << "done" << endl;

	cout << "running receiver with thread" << endl;
    receiver.start();
    testSender();
    sleep(1);
    receiver.stop();
    cout << "done " << endl << endl;

	cout << "exited cleanly";
    return 0;
}

void testTimeTag()
{
	TimeTag tagA;
    cout << "tagA is " << tagA.sec << " " << tagA.frac << endl;

	cout << "sleeping 5 seconds ..." << endl;
	sleep(5);

	TimeTag tagB;
    cout << "tagB is " << tagB.sec << " " << tagB.frac << endl;

	// check difference (sleep is not so accurate, so difference will be ~ 5 secs)
	cout << "tagB-tagA = " << tagB.diff(tagA) << endl;
}

void testSender()
{
	OscSender sender;
    
    sender.setup("127.0.0.1", 9990);
    
    // send a quick message
    sender << osc::BeginMessage("/test1")
           << (bool) 1 << 40.0f << (float) 1024.3434 << Nil() << (std::string) "string one" << "string two"
           << osc::EndMessage();
    sender.send();
    
    // send a midi message and a blob (binary data)
    osc::MidiMessage m;
    m.bytes[0] = 0x7F;
    m.bytes[1] = 0x90;
    m.bytes[2] = 0x3E;
    m.bytes[3] = 0x60;
    string blobData = "this is some blob data";
    sender << osc::BeginMessage("/test2")
           << m << Blob(blobData.c_str(), sizeof(char)*(blobData.length()+1))
           << osc::EndMessage();
    sender.send();
    
    // send a message with all types to be parsed on server
    sender << osc::BeginMessage("/test3")
        << true					// bool true
        << false				// bool false
        << 'c'					// char
        << Nil()				// nil
        << Infinitum()			// infinitum
        << (int32_t) 100		// int32
        << (int64_t) 200		// int32
        << (float) 123.45		// float
        << (double) 678.90		// double
        << "a string"			// string
        << Symbol("a symbol") 	// symbol (NULL-terminated C-string)
        << m					// midi message
        << TimeTag()			// time tag (right now)
        << Blob(blobData.c_str(), sizeof(char)*(blobData.length()+1)) // binary blob data
        << osc::EndMessage();
    sender.send();
    
    // send quit message
    sender << osc::BeginMessage("/quit") << osc::EndMessage();
    sender.send();
}
