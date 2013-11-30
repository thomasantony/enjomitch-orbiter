#ifndef DIALOGPID_H
#define DIALOGPID_H

class LaunchMFD;

class DialogPID
{
    public:
        void PIDTuningXY( LaunchMFD * launchMFD );
        void PIDTuningBank( LaunchMFD * launchMFD );

    protected:
    private:
        static bool clbkPIDTuningXY(void *id, char *str, void *usrdata);
        static bool clbkPIDTuningBank(void *id, char *str, void *usrdata);
};

#endif // DIALOGPID_H
