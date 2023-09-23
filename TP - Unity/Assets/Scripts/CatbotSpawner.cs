using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CatbotSpawner : MonoBehaviour
{
    public GameObject prefab;
    public int nbObjects = 5;
    public float minX = -23f;
    public float maxX = 23f;
    public float minZ = -23f;
    public float maxZ = 23f;
    public float y = 0f;
    void Start()
    {
        for (int i = 0; i < nbObjects; i++)
        {
            Vector3 randomPos = new Vector3(Random.Range(minX, maxX), y, Random.Range(minZ, maxZ));
            Instantiate(prefab, randomPos, Quaternion.identity);
        }
    }
    void Update()
    {
    }
}
