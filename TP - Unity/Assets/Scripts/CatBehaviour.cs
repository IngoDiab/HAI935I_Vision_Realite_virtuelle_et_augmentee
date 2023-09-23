using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CatBehaviour : MonoBehaviour
{
    [SerializeField] GameObject fx = null;
    AudioSource collisionSound = null;
    GameObject worldObject = null;

    // Start is called before the first frame update
    void Start()
    {
        worldObject = GameObject.Find("World");
        collisionSound = GameObject.Find("World").GetComponent<AudioSource>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag == "Player")
        {
            worldObject.SendMessage("AddHit");
            Destroy(gameObject);
            if (collisionSound) collisionSound.Play();
            if (fx) Instantiate(fx, transform.position, Quaternion.identity);
        }
    }
}
