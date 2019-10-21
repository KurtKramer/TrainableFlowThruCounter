namespace CounterApplication
{
  partial class SplashScreen
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose (bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose ();
      }
      base.Dispose (disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent ()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SplashScreen));
      this.SplashScreenTimer = new System.Windows.Forms.Timer(this.components);
      this.panel1 = new System.Windows.Forms.Panel();
      this.pictureBox2 = new System.Windows.Forms.PictureBox();
      this.panel1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
      this.SuspendLayout();
      // 
      // SplashScreenTimer
      // 
      this.SplashScreenTimer.Enabled = true;
      this.SplashScreenTimer.Tick += new System.EventHandler(this.SplashScreenTimer_Tick);
      // 
      // panel1
      // 
      this.panel1.BackColor = System.Drawing.Color.White;
      this.panel1.Controls.Add(this.pictureBox2);
      this.panel1.Location = new System.Drawing.Point(12, 12);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(589, 376);
      this.panel1.TabIndex = 1;
      // 
      // pictureBox2
      // 
      this.pictureBox2.Image = global::CounterApplication.Properties.Resources.ZOIS_Logo;
      this.pictureBox2.InitialImage = ((System.Drawing.Image)(resources.GetObject("pictureBox2.InitialImage")));
      this.pictureBox2.Location = new System.Drawing.Point(3, 3);
      this.pictureBox2.Name = "pictureBox2";
      this.pictureBox2.Size = new System.Drawing.Size(583, 370);
      this.pictureBox2.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
      this.pictureBox2.TabIndex = 1;
      this.pictureBox2.TabStop = false;
      // 
      // SplashScreen
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(617, 404);
      this.Controls.Add(this.panel1);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "SplashScreen";
      this.Text = "K-Square Software";
      this.panel1.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion
    private System.Windows.Forms.Timer SplashScreenTimer;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.PictureBox pictureBox2;
  }
}