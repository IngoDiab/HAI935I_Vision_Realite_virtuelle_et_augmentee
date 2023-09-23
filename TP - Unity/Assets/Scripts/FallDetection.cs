using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FallDetection : MonoBehaviour
{
    [SerializeField] Transform Spawn;
    void OnTriggerEnter(Collider other)
    {
        if (other.tag == "Player")
        {
            other.transform.position = Spawn.transform.position;
            other.transform.rotation = Spawn.transform.rotation;
        }
    }
}
