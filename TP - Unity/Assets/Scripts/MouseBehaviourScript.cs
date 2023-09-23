using System.Collections;
using UnityEngine;

public class MouseBehaviourScript : MonoBehaviour
{
    [SerializeField] float speed = 5.0f;
    //[HideInInspector] public float speed = 5.0f;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        float x = Input.GetAxis("Horizontal") * Time.deltaTime * speed;
        float z = Input.GetAxis("Vertical") * Time.deltaTime * speed;
        transform.Translate(x, 0, z);
    }
}
