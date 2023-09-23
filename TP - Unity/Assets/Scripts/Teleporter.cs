using System;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Teleporter : MonoBehaviour
{
    public Action OnEnableTeleporter = null;

    [SerializeField] string mSceneTeleport = "";
    bool mCanTeleport = false;

    private void Awake()
    {
        OnEnableTeleporter += () => mCanTeleport = true;
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnDestroy()
    {
        OnEnableTeleporter = null;
    }

    private void OnTriggerEnter(Collider other)
    {
        if (!mCanTeleport || other.tag != "Player" || mSceneTeleport == "") return;
        SceneManager.LoadScene(mSceneTeleport);
        GameVariables.currentTime = GameVariables.allowedTime;
    }
}
