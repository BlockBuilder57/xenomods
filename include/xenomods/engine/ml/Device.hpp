namespace ml {
    struct Device {
        struct InitParam {};
    };

    struct DeviceNx {
        static void initializeSDK(ml::Device::InitParam const&);
    };
}
