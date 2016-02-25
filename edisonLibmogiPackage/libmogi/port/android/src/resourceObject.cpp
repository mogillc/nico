
//#include <jni.h>
#include <mogi/port/android/resourceInterface.h>
#include <mogi/port/android/AndroidEnvironment.h>

std::string _getMogiResourceDirectory() {
	return Mogi::Android::Environment::getInstance().getResourceDirectory();
}

GLuint _loadTexture(const char* name, int glslVersion) {
	/*JNIEnv* env = Mogi::Android::Environment::getInstance().getJNIEnvironment();

	if (env) {
		jstring jtextureName = env->NewStringUTF(name); // convert the asset's name 

		Mogi::Android::JavaStaticClass javaStaticClass(env, "mogi/hexapod/jni/GL2JNILib");
		javaStaticClass.addMethod("testLoadTexture", "(Ljava/lang/String;)I");

		int textureID = -1;
		javaStaticClass.callMethod(&textureID, "testLoadTexture", jtextureName);

		return textureID;
	}

	return -1;*/

	Mogi::Android::Environment& androidEnv = Mogi::Android::Environment::getInstance();
	JNIEnv* env = androidEnv.getJNIEnvironment();
	std::string apiName = "mogi/hexapod/jni/TextureLoader";
	int textureID = -1;

	if (androidEnv.hasAPI(apiName)) {
		// Get reference to the texture loading api 
		Mogi::Android::JavaStaticClass* textureLoaderAPI = androidEnv.getJavaAPI(apiName);

		jstring jtextureName = env->NewStringUTF(name); // convert the asset's name 
		textureLoaderAPI->callMethod(&textureID, "testLoadTexture", jtextureName); // Call load method
	}

	return textureID;
}
