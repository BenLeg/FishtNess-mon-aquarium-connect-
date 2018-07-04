using Constellation;
using Constellation.Package;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace IntegrationV2
{
    public class Program : PackageBase
    {

        private DateTime dateProcessedFeeding = DateTime.MinValue;
        private DateTime dateProcessedSwitchON = DateTime.MinValue;
        private DateTime dateProcessedSwitchOFF = DateTime.MinValue;
        private DateTime dateProcessedPushBullet1 = DateTime.MinValue;
        private DateTime dateProcessedPushBullet2 = DateTime.MinValue;
        private DateTime dateProcessedPushBullet3 = DateTime.MinValue;
        private DateTime dateProcessedPushBullet4 = DateTime.MinValue;


        static void Main(string[] args)
        {
            PackageHost.Start<Program>(args);
        }


        //Inserez votre sentinel à la place de *
        [StateObjectLink("*", "FishtNess", "Temperature")]
        private StateObjectNotifier Temperature { get; set; }

        [StateObjectLink("*", "FishtNess", "Acidity")]
        private StateObjectNotifier Acidity { get; set; }

        long T;
        long pH;

        public override void OnStart()
        {
            PackageHost.WriteInfo("Package starting - IsRunning: {0} - IsConnected: {1}", PackageHost.IsRunning, PackageHost.IsConnected);

            while (PackageHost.IsRunning)
            {
                int Heure = DateTime.Now.Hour;
                int Minute = DateTime.Now.Minute;

                if (DateTime.Now.Date != dateProcessedFeeding.Date && Heure == 13 && Minute == 30)
                {
                    dateProcessedFeeding = DateTime.Now;
                    PackageHost.SendMessage(MessageScope.Create("FishtNess"), "Feeding", null);
                }

                if (DateTime.Now.Date != dateProcessedSwitchON.Date && Heure == 11 && Minute == 30)
                {
                    dateProcessedSwitchON = DateTime.Now;
                    PackageHost.SendMessage(MessageScope.Create("FishtNess"), "SwitchState", true);
                }

                if (DateTime.Now.Date != dateProcessedSwitchOFF.Date && Heure == 21 && Minute == 30)
                {
                    dateProcessedSwitchOFF = DateTime.Now;
                    PackageHost.SendMessage(MessageScope.Create("FishtNess"), "SwitchState", false);
                }


                if (DateTime.Now.Date != dateProcessedPushBullet1.Date && Heure == 8)
                {
                    dateProcessedPushBullet1 = DateTime.Now;
                    PackageHost.CreateMessageProxy("PushBullet").PushNote("ATTENTION", "La température de l'aquarium est : {0}°C.", this.Temperature.DynamicValue);
                }

                else if (DateTime.Now.Date != dateProcessedPushBullet2.Date && Heure == 15)
                {
                    dateProcessedPushBullet2 = DateTime.Now;
                    PackageHost.CreateMessageProxy("PushBullet").PushNote("ATTENTION", "La température de l'aquarium est : {0}°C.", this.Temperature.DynamicValue);
                }

                else if (DateTime.Now.Date != dateProcessedPushBullet3.Date && Heure == 8)
                {
                    dateProcessedPushBullet3 = DateTime.Now;
                    PackageHost.CreateMessageProxy("PushBullet").PushNote("ATTENTION", "L'acidite (pH) de l'aquarium est : {0}.", this.Acidity.DynamicValue);
                }

                else if (DateTime.Now.Date != dateProcessedPushBullet4.Date && Heure == 15)
                {
                    dateProcessedPushBullet4 = DateTime.Now;
                    PackageHost.CreateMessageProxy("PushBullet").PushNote("ATTENTION", "L'acidite (pH) de l'aquarium est : {0}.", this.Acidity.DynamicValue);
                }



                Thread.Sleep(PackageHost.GetSettingValue<int>("Interval"));
            }

        }
    }
}