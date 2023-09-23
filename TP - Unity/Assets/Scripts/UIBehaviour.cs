using System.Collections;
using TMPro;
using UnityEngine;
using UnityEngine.SceneManagement;

public class UIBehaviour : MonoBehaviour
{
    TMP_Text headText;
    TMP_Text timerText;
    int nbHeads = 0;

    [SerializeField] int mNbCatbotUnlockTeleporter = 3;
    [SerializeField] Teleporter mTeleporter = null;

    void Start()
    {
        GameObject _lblCats = GameObject.Find("lblCats");
        if(_lblCats) headText = _lblCats.GetComponent<TMPro.TMP_Text>();

        GameObject _lblTime = GameObject.Find("lblTime");
        if(_lblTime) timerText = _lblTime.GetComponent<TMPro.TMP_Text>();
        StartCoroutine(TimerTick());
    }
    void Update()
    {
    }

    public void AddHit()
    {
        nbHeads++;
        headText.text = "BobHeads: " + nbHeads;
        if (nbHeads >= mNbCatbotUnlockTeleporter) mTeleporter.OnEnableTeleporter.Invoke();
    }

    IEnumerator TimerTick()
    {
        while (GameVariables.currentTime > 0)
        {
            // attendre une seconde
            yield return new WaitForSeconds(1);
            GameVariables.currentTime--;
            timerText.text = "Time :" + GameVariables.currentTime.ToString();
        }
        // game over
        SceneManager.LoadScene(SceneManager.GetActiveScene().name);
        GameVariables.currentTime = GameVariables.allowedTime;
    }
}
