
/*
using AForge.Video.DirectShow;
using System;
using System.Drawing;
using System.Windows.Forms;

using Microsoft.DirectX.DirectSound;
using System;
using System.Threading;
using System.Windows.Forms;


namespace CameraTest
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        private void button5_Click(object sender, EventArgs e)
        {
            Form2 form = new Form2();
            form.Text = "����";
            form.Show();
        }
        private void button1_Click(object sender, EventArgs e)
        {
            Form3 form = new Form3();
            form.Text = "����";
            form.Show();
        }
    }
}

namespace CameraTest
{
    public partial class Form2 : Form
    {
        private FilterInfoCollection videoDevices;
        public Form2()
        {
            InitializeComponent();
        }
        private void videoSourcePlayer()
        {
            VideoCaptureDevice videoCapture = new VideoCaptureDevice(videoDevices[comboBox1.SelectedIndex].MonikerString);
#pragma warning disable CS0612 // ���ͻ��Ա�ѹ�ʱ
            videoCapture.DesiredFrameSize = new Size(320, 240);
#pragma warning restore CS0612 // ���ͻ��Ա�ѹ�ʱ
#pragma warning disable CS0612 // ���ͻ��Ա�ѹ�ʱ
            videoCapture.DesiredFrameRate = 1;
#pragma warning restore CS0612 // ���ͻ��Ա�ѹ�ʱ
            videoPlayer1.VideoSource = videoCapture;
            videoPlayer1.Start();
        }
        private void button4_Click_1(object sender, EventArgs e)
        {
            try
            {
                //ö����Ƶ�����豸
                videoDevices = new FilterInfoCollection(FilterCategory.VideoInputDevice);
                if (videoDevices.Count == 0)
                {
                    throw new ApplicationException();
                }
                foreach(FilterInfo device in videoDevices)
                {
                    comboBox1.Items.Add(device.Name);
                }
                comboBox1.SelectedIndex = 0;
            }
            catch
            {
                comboBox1.Items.Add("û����Ƶ�豸��");
                videoDevices = null;
            }
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            videoSourcePlayer();
        }

        private void button3_Click_1(object sender, EventArgs e)
        {
            //����

            if (videoPlayer1 == null)
                return;
            Bitmap bitmap = videoPlayer1.GetCurrentVideoFrame();
            saveFileDialog1.Filter = "Jpg ͼƬ|*.jpg|Bmp ͼƬ|*.bmp|Gif ͼƬ|*.gif|Png ͼƬ|*.png|Wmf  ͼƬ|*.wmf";
            saveFileDialog1.FilterIndex = 0;
            saveFileDialog1.RestoreDirectory = true;//����Ի����Ƿ�����ϴδ򿪵�Ŀ¼
            saveFileDialog1.CheckPathExists = true;//���Ŀ¼
            saveFileDialog1.FileName = System.DateTime.Now.ToString("yyyyMMddHHmmss") + "-"; ;//����Ĭ���ļ���
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                bitmap.Save(saveFileDialog1.FileName, System.Drawing.Imaging.ImageFormat.Jpeg);// imageΪҪ�����ͼƬ
                MessageBox.Show(this, "ͼƬ����ɹ���", "��Ϣ��ʾ");
            }
            bitmap.Dispose();
        }
        private void button2_Click_1(object sender, EventArgs e)
        {
            videoPlayer1.SignalToStop();
            videoPlayer1.WaitForStop();
            this.Close();
        }
        private void Form2_Load(object sender, EventArgs e)
        {}
    }
}

namespace CameraTest
{
    public partial class Form3 : Form
    {
        private const int SAMPLES = 8;
        private static int deviceIndex = -1;
        private CaptureBuffer buffer = null;
        private static Capture cap = null;
        private string deviceName = "û�м�⵽��Ƶ�����豸";

        ///
        // *****���Ĵ�
        ///
        private static AutoCompleteStringCollection deviceNames;
        private Thread liveVolumeThread;
        private int sampleDelay = 100;
        private int frameDelay = 10;
        private static int[] SAMPLE_FORMAT_ARRAY = { SAMPLES, 2, 1 };
        public Form3()
        {
            InitializeComponent();
            progressBar1.Maximum = Int16.MaxValue + 1;
            CheckForIllegalCrossThreadCalls = false;
        }
        /// <summary>
        /// ������˷��б�
        /// </summary>

        public void Stop()
        {
            if (liveVolumeThread != null)
            {
                liveVolumeThread.Abort();
                liveVolumeThread.Join();
                liveVolumeThread = null;
            }

            if (buffer != null)
            {
                if (buffer.Capturing)
                {
                    buffer.Stop();
                }

                buffer.Dispose();
                buffer = null;
            }
        }

        public void Start()
        {
            Stop();
            if (deviceIndex != -1)
            {
                // capture ���� ������˷��豸

                ///
                // *****���Ĵ�
                ///
                CaptureDevicesCollection audioDevices = new CaptureDevicesCollection();
                cap = new Capture(audioDevices[deviceIndex].DriverGuid);
                // ������ ��������Ϣ ������
                CaptureBufferDescription desc = new CaptureBufferDescription();
                WaveFormat wf = new WaveFormat();
                wf.BitsPerSample = 16;
                wf.SamplesPerSecond = 44100;
                wf.Channels = 2;
                // ���ݵ���С��ԭ�ӵ�Ԫ
                wf.BlockAlign = (short)(wf.Channels * wf.BitsPerSample / 8);
                // ��λ��������ֽ���
                wf.AverageBytesPerSecond = wf.BlockAlign * wf.SamplesPerSecond;
                // δ��ѹ����PCM
                wf.FormatTag = WaveFormatTag.Pcm;

                desc.Format = wf;
                desc.BufferBytes = SAMPLES * wf.BlockAlign;
                // ���� capturebuffer����
                buffer = new CaptureBuffer(desc, cap);
                // ��׽����������
                buffer.Start(true);

                liveVolumeThread = new Thread(new ThreadStart(updateProgress));
                liveVolumeThread.Priority = ThreadPriority.Lowest;
                liveVolumeThread.Start();
            }
        }
        private void updateProgress()
        {
            while (true)
            {
                int tempFrameDelay = frameDelay;
                int tempSampleDelay = sampleDelay;
                ///
                // *****���Ĵ�
                ///
                label1.Text = "0";
                Array samples = buffer.Read(0, typeof(Int16), LockFlag.FromWriteCursor, SAMPLE_FORMAT_ARRAY);
                int goal = 0;
                for (int i = 0; i < SAMPLES; i++)
                {
                    goal += (Int16)samples.GetValue(i, 0, 0);
                }
                goal = (int)Math.Abs(goal / SAMPLES);
                double range = goal - progressBar1.Value;
                double exactValue = progressBar1.Value;
                double stepSize = range / tempSampleDelay * tempFrameDelay;
                if (Math.Abs(stepSize) < .01)
                {
                    stepSize = Math.Sign(range) * .01;
                }
                double absStepSize = Math.Abs(stepSize);

                if ((progressBar1.Value == goal))
                {
                    Thread.Sleep(tempSampleDelay);
                }
                else
                {
                    do
                    {
                        if (progressBar1.Value != goal)
                        {
                            if (absStepSize < Math.Abs(goal - progressBar1.Value))
                            {
                                exactValue += stepSize;

                                progressBar1.Value = (int)Math.Round(exactValue);
                                ///
                                // *****���Ĵ�
                                ///

                                if (int.Parse(label1.Text) > (int)Math.Round(exactValue) + 100 || int.Parse(label1.Text) < (int)Math.Round(exactValue) - 100)
                                {
                                    label1.Text = Math.Round(exactValue).ToString();
                                    Application.DoEvents();
                                }
                            }
                            else
                            {
                                progressBar1.Value = goal;
                            }
                        }
                        Thread.Sleep(tempFrameDelay);
                    } while ((progressBar1.Value != goal));
                }
            }
        }
        private void button2_Click(object sender, EventArgs e)
        {
            Stop();
        }
        private void button3_Click(object sender, EventArgs e)
        {
            CaptureDevicesCollection audioDevices = new CaptureDevicesCollection();
            deviceNames = new AutoCompleteStringCollection();
            for (int i = 0; i < audioDevices.Count; i++)
            {
                deviceNames.Add(audioDevices[i].Description);

                comboBox1.Items.Add(deviceNames[i].ToString());
                comboBox1.SelectedIndex = 0;
            }
            if (deviceNames.Count < 0)
            {
                comboBox1.Items.Clear();
                comboBox1.Items.Add(deviceName);
            }
        }
        private void button1_Click(object sender, EventArgs e)
        {
            deviceIndex = comboBox1.SelectedIndex;
            Start();
        }
        private void button4_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        private void Form3_Load(object sender, EventArgs e)
        { }
    }
}

*/