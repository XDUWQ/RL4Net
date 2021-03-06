/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 Technische Universität Berlin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Piotr Gawlowicz <gawlowicz@tkn.tu-berlin.de>
 */

#include "myenv.h"
#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/node-list.h"
#include "ns3/log.h"
#include <sstream>
#include <iostream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MyOpenEnv");

NS_OBJECT_ENSURE_REGISTERED (MyOpenEnv);

MyOpenEnv::MyOpenEnv ()
{
  NS_LOG_FUNCTION (this);
  m_interval = Seconds(0.1);

  Simulator::Schedule (Seconds(0.0), &MyOpenEnv::ScheduleNextStateRead, this);
}

MyOpenEnv::MyOpenEnv (Time stepTime)
{
  NS_LOG_FUNCTION (this);
  m_interval = stepTime;

  Simulator::Schedule (Seconds(0.0), &MyOpenEnv::ScheduleNextStateRead, this);
}

void
MyOpenEnv::ScheduleNextStateRead ()
{
  NS_LOG_FUNCTION (this);
  Simulator::Schedule (m_interval, &MyOpenEnv::ScheduleNextStateRead, this);
  Notify();
}

MyOpenEnv::~MyOpenEnv ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
MyOpenEnv::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyOpenEnv")
    .SetParent<OpenEnvAbstract> ()
    .SetGroupName ("OpenEnv")
    .AddConstructor<MyOpenEnv> ()
  ;
  return tid;
}

void
MyOpenEnv::DoDispose ()
{
  NS_LOG_FUNCTION (this);
}

/*
Define observation space
*/
Ptr<OpenEnvSpace>
MyOpenEnv::GetObservationSpace()
{
  uint32_t nodeNum = 5;
  float low = 0.0;
  float high = 10.0;
  std::vector<uint32_t> shape = {nodeNum,};
  std::string dtype = TypeNameGet<uint32_t> ();

  Ptr<OpenEnvDiscreteSpace> discrete = CreateObject<OpenEnvDiscreteSpace> (nodeNum);
  Ptr<OpenEnvBoxSpace> box = CreateObject<OpenEnvBoxSpace> (low, high, shape, dtype);

  Ptr<OpenEnvDictSpace> space = CreateObject<OpenEnvDictSpace> ();
  space->Add("box", box);
  space->Add("discrete", discrete);

  NS_LOG_UNCOND ("MyGetObservationSpace: " << space);
  return space;
}

/*
Define action space
*/
Ptr<OpenEnvSpace>
MyOpenEnv::GetActionSpace()
{
  uint32_t nodeNum = 5;
  float low = 0.0;
  float high = 10.0;
  std::vector<uint32_t> shape = {nodeNum,};
  std::string dtype = TypeNameGet<uint32_t> ();

  Ptr<OpenEnvDiscreteSpace> discrete = CreateObject<OpenEnvDiscreteSpace> (nodeNum);
  Ptr<OpenEnvBoxSpace> box = CreateObject<OpenEnvBoxSpace> (low, high, shape, dtype);

  Ptr<OpenEnvDictSpace> space = CreateObject<OpenEnvDictSpace> ();
  space->Add("box", box);
  space->Add("discrete", discrete);

  NS_LOG_UNCOND ("MyGetActionSpace: " << space);
  return space;
}

/*
Define game over condition
*/
bool
MyOpenEnv::GetGameOver()
{
  bool isGameOver = false;
  bool test = false;
  static float stepCounter = 0.0;
  stepCounter += 1;
  if (stepCounter == 10 && test) {
      isGameOver = true;
  }
  NS_LOG_UNCOND ("MyGetGameOver: " << isGameOver);
  return isGameOver;
}

/*
Collect observations
*/
Ptr<OpenEnvDataContainer>
MyOpenEnv::GetObservation()
{
  uint32_t nodeNum = 5;
  uint32_t low = 0.0;
  uint32_t high = 10.0;
  Ptr<UniformRandomVariable> rngInt = CreateObject<UniformRandomVariable> ();

  std::vector<uint32_t> shape = {nodeNum,};
  Ptr<OpenEnvBoxContainer<uint32_t> > box = CreateObject<OpenEnvBoxContainer<uint32_t> >(shape);

  // generate random data
  for (uint32_t i = 0; i<nodeNum; i++){
    uint32_t value = rngInt->GetInteger(low, high);
    box->AddValue(value);
  }

  Ptr<OpenEnvDiscreteContainer> discrete = CreateObject<OpenEnvDiscreteContainer>(nodeNum);
  uint32_t value = rngInt->GetInteger(low, high);
  discrete->SetValue(value);

  Ptr<OpenEnvTupleContainer> data = CreateObject<OpenEnvTupleContainer> ();
  data->Add(box);
  data->Add(discrete);

  // Print data from tuple
  Ptr<OpenEnvBoxContainer<uint32_t> > mbox = DynamicCast<OpenEnvBoxContainer<uint32_t> >(data->Get(0));
  Ptr<OpenEnvDiscreteContainer> mdiscrete = DynamicCast<OpenEnvDiscreteContainer>(data->Get(1));
  NS_LOG_UNCOND ("MyGetObservation: " << data);
  NS_LOG_UNCOND ("---" << mbox);
  NS_LOG_UNCOND ("---" << mdiscrete);

  return data;
}

/*
Define reward function
*/
float
MyOpenEnv::GetReward()
{
  static float reward = 0.0;
  reward += 1;
  return reward;
}

/*
Define extra info. Optional
*/
std::string
MyOpenEnv::GetExtraInfo()
{
  std::string myInfo = "testInfo";
  myInfo += "|123";
  NS_LOG_UNCOND("MyGetExtraInfo: " << myInfo);
  return myInfo;
}

/*
Execute received actions
*/
bool
MyOpenEnv::ExecuteActions(Ptr<OpenEnvDataContainer> action)
{
  Ptr<OpenEnvDictContainer> dict = DynamicCast<OpenEnvDictContainer>(action);
  Ptr<OpenEnvBoxContainer<uint32_t> > box = DynamicCast<OpenEnvBoxContainer<uint32_t> >(dict->Get("box"));
  Ptr<OpenEnvDiscreteContainer> discrete = DynamicCast<OpenEnvDiscreteContainer>(dict->Get("discrete"));

  NS_LOG_UNCOND ("MyExecuteActions: " << action);
  NS_LOG_UNCOND ("---" << box);
  NS_LOG_UNCOND ("---" << discrete);
  return true;
}

} // ns3 namespace