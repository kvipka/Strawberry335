/**
 * @file
 * @author TehKevin for Laenalith-WoW
 * @date 26.08.2014
 *
 * Contact kevin.unegg@ainet.at
 * Website: http://www.laenalith-wow.com/
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @copyright Copyright 2014 Laenalith-WoW. All rights reserved.
 *
 * @description
 *
 * Base class for all encounters in the instance "Trial Of The Champion"
 * Used to handle intro and pre-encounters
 */

#ifndef _ENCOUNTER_TRIAL_OF_THE_CHAMPION_H
#define _ENCOUNTER_TRIAL_OF_THE_CHAMPION_H

#include "ace/Assert.h"
#include "AutoPtr.h"


#define EVENT_SCHEDULE_START        1
#define EVENT_SCHEDULE_START_TIMER  1 * IN_MILLISECONDS

#define GET_CREATURE_ANNOUNCER(x)   x = pInstanceMap->GetCreature(pInstance->GetData64(NPC_JAEREN))
#define GET_CREATURE_TRIGGER(x)     x = pInstanceMap->GetCreature(pInstance->GetData64(NPC_ORIENTATION_TRIGGER))
#define SUMMON_CREATURE_TRIGGER(x)  pInstanceMap->SummonCreatureOnMap(NPC_ORIENTATION_TRIGGER, x, TEMPSUMMON_MANUAL_DESPAWN, 0)

enum Encounter_State
{
    ENCOUNTER_STATE_NOTHING         = 0, // Encounter initalized state
    ENCOUNTER_STATE_INTRO           = 1, // Encounter needs update and calls executeIntro
    ENCOUNTER_STATE_PRE_ENCOUNTER   = 2, // Encounter needs update and calls executePreEncounter (optional)
    ENCOUNTER_STATE_ENCOUNTER       = 3  // Encounter handling now performed from CreatureScript
};

enum Encounter_Data
{
    ENCOUNTER_PREFIX                = 1000
};

class InstanceScript;
typedef std::map<uint32, uint32> EncounterDataMap;

class Encounter_TrialOfTheChampion
{
    public:

        Encounter_TrialOfTheChampion(InstanceScript* instance, bool HasPreEncounter)
            : currentState(ENCOUNTER_STATE_NOTHING), pInstance(instance), hasPreEncounter(HasPreEncounter)
        {
            sLog->outDebug(LOG_FILTER_TSCR, "Call: Encounter_TrialOfTheChampion::Encounter_TrialOfTheChampion(%u, %u)", &instance, HasPreEncounter);

            ASSERT(pInstance.get() != NULL);
            pInstanceMap = pInstance.get()->instance;
        }

        virtual ~Encounter_TrialOfTheChampion()
        {
            sLog->outDebug(LOG_FILTER_TSCR, "Call: Encounter_TrialOfTheChampion::~Encounter_TrialOfTheChampion()");

            // Give ownership back
            pInstance.release();
        }

        /**
         * Called to set encounter active
         *
         * @param eventID : Start eventId which should be executed (default = 1)
         */
        void Start(uint32 eventID = EVENT_SCHEDULE_START);

        /**
         * Used to get data - Between instance and encounter
         *
         * @param type : Key of the data map
         * @param data : Reference to uint32 var
         * @return Key is valid
         */
         bool GetData(uint32 type, uint32& data);

        /**
         * Used to set data - Between instance and encounter
         *
         * +Calls event onDataChanged
         * @param type : Key of the data map
         * @param data : Value of the key
         */
         void SetData(uint32 type, uint32 data);

        /**
         * Used to increase data - Between instance and encounter
         *
         * +Calls event onDataChanged
         * @param type : Key of the data map
         * @param data : Increase by value
         */
         void IncreaseData(uint32 type, uint32 data);

        /**
         * Used for interaction between instance and encounter
         *
         * -Pure virtual
         */
        virtual void DoAction(int32 action) {};

        /**
         * Event which will be called when all players are dead
         *
         * -Pure virtual
         * +Reset encounter
         */
        virtual void OnFail() {}

        /** Returns current state of encounter */
        const Encounter_State GetState() const{
            return currentState;
        }

        /** Returns if object needs update */
        const bool IsActive() const{
            return (currentState != ENCOUNTER_STATE_NOTHING && currentState != ENCOUNTER_STATE_ENCOUNTER);
        }

        /**
         * Update function of encounter every server tick
         *
         * -Has to be called from InstanceScript::Update
         * -Can be overwritten
         * +Update EventMap
         * +Call execute function of current state
         */
        virtual void Update(const uint32 diff);

    protected:

        /**
         * Event which will be called when SetData has been called
         *
         * -Pure virtual
         * @param type : Key of the data map
         */
        virtual void onDataChanged(uint32 type) {}

        /**
         * Event which will be called when encounter starts
         *
         * -Pure virtual
         */
        virtual void onStart() {}

        /**
         * Will be called from update if state is ENCOUNTER_STATE_INTRO
         *
         * -Pure virtual - Has to be overwritten
         */
        virtual void executeIntro(uint32 eventID) = 0;

        /**
         * Will be called from update if state is ENCOUNTER_STATE_PRE_ENCOUNTER
         *
         * -Pure virtual
         */
        virtual void executePreEncounter(uint32 eventID) {};

        /**
         *
         * Called to perform next phase of encounter
         *
         * -Next phase is depending on state and hasPreEncounter
         * -Can only be called after Start() has been called
         * @return : Successfully started next phase
         */
        bool startNext();

        /** Called to open the main door */
        void openDoor();

        /** Called to close the main door */
        void closeDoor();

        std::auto_ptr<InstanceScript> pInstance;
        Map* pInstanceMap;
        EventMap eventMap;

    private:

        EncounterDataMap dataMap;
        Encounter_State currentState;
        bool hasPreEncounter;
};


#endif // _ENCOUNTER_TRIAL_OF_THE_CHAMPION_H
