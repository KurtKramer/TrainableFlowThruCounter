namespace CounterFileViewer
{
  partial class CameraLineDisplay
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
      this.exitButton = new System.Windows.Forms.Button ();
      this.pictureScrollBar = new System.Windows.Forms.HScrollBar ();
      this.cameraLineWindow = new System.Windows.Forms.Panel ();
      this.BottomControlsPannel = new System.Windows.Forms.Panel ();
      this.MediumSmootingWindowLabel = new System.Windows.Forms.Label ();
      this.MediumSmootingWindow = new System.Windows.Forms.NumericUpDown ();
      this.ScalingRationLabel = new System.Windows.Forms.Label ();
      this.ScalingRatio = new System.Windows.Forms.NumericUpDown ();
      this.BottomControlsPannel.SuspendLayout ();
      ((System.ComponentModel.ISupportInitialize)(this.MediumSmootingWindow)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.ScalingRatio)).BeginInit ();
      this.SuspendLayout ();
      // 
      // exitButton
      // 
      this.exitButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.exitButton.Location = new System.Drawing.Point (860, 19);
      this.exitButton.Name = "exitButton";
      this.exitButton.Size = new System.Drawing.Size (82, 36);
      this.exitButton.TabIndex = 300;
      this.exitButton.Text = "Exit";
      this.exitButton.UseVisualStyleBackColor = true;
      this.exitButton.Click += new System.EventHandler (this.exitButton_Click_1);
      // 
      // pictureScrollBar
      // 
      this.pictureScrollBar.LargeChange = 512;
      this.pictureScrollBar.Location = new System.Drawing.Point (2, 459);
      this.pictureScrollBar.Maximum = 4096;
      this.pictureScrollBar.Name = "pictureScrollBar";
      this.pictureScrollBar.Size = new System.Drawing.Size (970, 22);
      this.pictureScrollBar.TabIndex = 5;
      this.pictureScrollBar.ValueChanged += new System.EventHandler (this.pictureScrollBar_ValueChanged_1);
      // 
      // cameraLineWindow
      // 
      this.cameraLineWindow.BackColor = System.Drawing.SystemColors.WindowText;
      this.cameraLineWindow.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.cameraLineWindow.Font = new System.Drawing.Font ("Arial", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.cameraLineWindow.Location = new System.Drawing.Point (2, 2);
      this.cameraLineWindow.Name = "cameraLineWindow";
      this.cameraLineWindow.Size = new System.Drawing.Size (970, 454);
      this.cameraLineWindow.TabIndex = 6;
      this.cameraLineWindow.Paint += new System.Windows.Forms.PaintEventHandler (this.cameraLineWindow_Paint);
      // 
      // BottomControlsPannel
      // 
      this.BottomControlsPannel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.BottomControlsPannel.Controls.Add (this.MediumSmootingWindowLabel);
      this.BottomControlsPannel.Controls.Add (this.MediumSmootingWindow);
      this.BottomControlsPannel.Controls.Add (this.ScalingRationLabel);
      this.BottomControlsPannel.Controls.Add (this.ScalingRatio);
      this.BottomControlsPannel.Controls.Add (this.exitButton);
      this.BottomControlsPannel.Location = new System.Drawing.Point (2, 484);
      this.BottomControlsPannel.Name = "BottomControlsPannel";
      this.BottomControlsPannel.Size = new System.Drawing.Size (964, 67);
      this.BottomControlsPannel.TabIndex = 225;
      // 
      // MediumSmootingWindowLabel
      // 
      this.MediumSmootingWindowLabel.AutoSize = true;
      this.MediumSmootingWindowLabel.Location = new System.Drawing.Point (98, 15);
      this.MediumSmootingWindowLabel.Name = "MediumSmootingWindowLabel";
      this.MediumSmootingWindowLabel.Size = new System.Drawing.Size (67, 13);
      this.MediumSmootingWindowLabel.TabIndex = 302;
      this.MediumSmootingWindowLabel.Text = "Med Smooth";
      this.MediumSmootingWindowLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // MediumSmootingWindow
      // 
      this.MediumSmootingWindow.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F);
      this.MediumSmootingWindow.Location = new System.Drawing.Point (101, 30);
      this.MediumSmootingWindow.Maximum = new decimal (new int[] {
            50,
            0,
            0,
            0});
      this.MediumSmootingWindow.Name = "MediumSmootingWindow";
      this.MediumSmootingWindow.Size = new System.Drawing.Size (62, 22);
      this.MediumSmootingWindow.TabIndex = 301;
      this.MediumSmootingWindow.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // ScalingRationLabel
      // 
      this.ScalingRationLabel.AutoSize = true;
      this.ScalingRationLabel.Location = new System.Drawing.Point (17, 15);
      this.ScalingRationLabel.Name = "ScalingRationLabel";
      this.ScalingRationLabel.Size = new System.Drawing.Size (42, 13);
      this.ScalingRationLabel.TabIndex = 226;
      this.ScalingRationLabel.Text = "Scaling";
      this.ScalingRationLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // ScalingRatio
      // 
      this.ScalingRatio.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F);
      this.ScalingRatio.Location = new System.Drawing.Point (20, 30);
      this.ScalingRatio.Maximum = new decimal (new int[] {
            32,
            0,
            0,
            0});
      this.ScalingRatio.Minimum = new decimal (new int[] {
            1,
            0,
            0,
            0});
      this.ScalingRatio.Name = "ScalingRatio";
      this.ScalingRatio.Size = new System.Drawing.Size (55, 22);
      this.ScalingRatio.TabIndex = 230;
      this.ScalingRatio.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.ScalingRatio.Value = new decimal (new int[] {
            1,
            0,
            0,
            0});
      this.ScalingRatio.ValueChanged += new System.EventHandler (this.ScalingRatio_ValueChanged);
      // 
      // CameraLineDisplay
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (984, 552);
      this.Controls.Add (this.BottomControlsPannel);
      this.Controls.Add (this.cameraLineWindow);
      this.Controls.Add (this.pictureScrollBar);
      this.MinimumSize = new System.Drawing.Size (1000, 590);
      this.Name = "CameraLineDisplay";
      this.Text = "CameraLineDisplay";
      this.Load += new System.EventHandler (this.CameraLineDisplay_Load);
      this.SizeChanged += new System.EventHandler (this.CameraLineDisplay_SizeChanged);
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler (this.CameraLineDisplay_FormClosing);
      this.BottomControlsPannel.ResumeLayout (false);
      this.BottomControlsPannel.PerformLayout ();
      ((System.ComponentModel.ISupportInitialize)(this.MediumSmootingWindow)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.ScalingRatio)).EndInit ();
      this.ResumeLayout (false);

    }

    #endregion

    private System.Windows.Forms.Button exitButton;
    private System.Windows.Forms.HScrollBar pictureScrollBar;
    private System.Windows.Forms.Panel cameraLineWindow;
    private System.Windows.Forms.Panel BottomControlsPannel;
    private System.Windows.Forms.NumericUpDown ScalingRatio;
    private System.Windows.Forms.Label ScalingRationLabel;
    private System.Windows.Forms.Label MediumSmootingWindowLabel;
    private System.Windows.Forms.NumericUpDown MediumSmootingWindow;
  }
}