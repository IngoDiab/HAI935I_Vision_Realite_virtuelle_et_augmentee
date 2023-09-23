using UnityEngine;

public class Collectible : MonoBehaviour
{
    [SerializeField] float mSpeedRotation = 10;

    [SerializeField] float mSpeedUpDown = 10;
    [SerializeField] float mOffsetUp = 1;

    [SerializeField] Teleporter mTeleporter = null;

    Vector3 mStartPos;
    Vector3 mEndPos;
    bool mIsGoingUp = true;

    // Start is called before the first frame update
    void Start()
    {
        mStartPos = transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        UpDown();
        Rotate();
    }

    void UpDown()
    {
        mEndPos = mStartPos + transform.up * mOffsetUp;
        if (mIsGoingUp && (transform.position - mEndPos).magnitude < .1f) mIsGoingUp = false;
        else if (!mIsGoingUp && (transform.position - mStartPos).magnitude < .1f) mIsGoingUp = true;
        transform.position = mIsGoingUp ? Vector3.MoveTowards(transform.position, mEndPos, mSpeedUpDown * Time.deltaTime) : Vector3.MoveTowards(transform.position, mStartPos, mSpeedUpDown * Time.deltaTime);
    }

    void Rotate()
    {
        Vector3 _current = transform.eulerAngles;
        _current.y += mSpeedRotation * Time.deltaTime;
        transform.eulerAngles = _current;
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag != "Player") return;
        mTeleporter.OnEnableTeleporter.Invoke();
        Destroy(gameObject);
    }
}
