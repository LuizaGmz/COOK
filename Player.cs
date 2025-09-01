using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
 
public class Player : MonoBehaviour
{
    public Entity entity;
 
    [Header("Player Regen System")]
    public bool regenHPEnabled = true;
    public float regenHPTime = 5f;
    public int regenHPValue = 5;
    
 
    [Header("Game Manager")]
    public GameManager manager;
 
    [Header("Player UI")]
    public Slider health;

 
    void Start()
    {
        if(manager == null)
        {
            Debug.LogError("Você precisa anexar o game manager aqui no player");
            return;
        }    
 
        entity.maxHealth = manager.CalculateHealth(this);
       
 
        // teste
        
 
        entity.currentHealth = entity.maxHealth;
        health.maxValue = entity.maxHealth;
        health.value = health.maxValue;
 
        
 
        // iniciar o regenhealth
        StartCoroutine(RegenHealth());
        
    }
 
    private void Update() 
    {
        health.value = entity.currentHealth;
    
 
        // teste
        if(Input.GetKeyDown(KeyCode.Space))
        {
            entity.currentHealth -= 10;
            
        }
    }
 
    IEnumerator RegenHealth()
    {
        while(true) // loop infinito
        {
            if(regenHPEnabled)
            {
                if(entity.currentHealth <  entity.maxHealth)
                {
                    Debug.LogFormat("Recuperando HP do jogador");
                    entity.currentHealth += regenHPValue;
                    yield return new WaitForSeconds(regenHPTime);
                }
                else
                {
                    yield return null;
                }
            }
            else
            {
                yield return null;
            }
        }
    }
 

 
}
