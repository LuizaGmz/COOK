using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
 
public class GameManager : MonoBehaviour
{
    public Int32 CalculateHealth(Player player)
    {
        
        Int32 result = 10 + 10;
        Debug.LogFormat("CalculateHealth: {0}", result);
        return result;
    }
 
  
 
}
