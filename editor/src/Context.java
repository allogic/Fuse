class NativeContext {

    static {
        System.loadLibrary("engine");
    }

    public NativeContext() {
        System.out.println("Constructing native context");
    }

    public native long contextCreate(int width, int height, boolean standalone);
    public native boolean contextIsRunning(long context);
	public native void contextBeginFrame(long context);
    public native void contextEndFrame(long context);
    public native void contextDestroy(long context);

}