namespace LarcosCounter
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
      this.components = new System.ComponentModel.Container ();
      this.exitButton = new System.Windows.Forms.Button ();
      this.refreshButton = new System.Windows.Forms.Button ();
      this.pictureScrollBar = new System.Windows.Forms.HScrollBar ();
      this.cameraLineWindow = new System.Windows.Forms.Panel ();
      this.AutoRefreshButton = new System.Windows.Forms.Button ();
      this.autoRefreshTimer = new System.Windows.Forms.Timer (this.components);
      this.RequestedDigitalGain = new System.Windows.Forms.NumericUpDown ();
      this.DigitalGainLabel = new System.Windows.Forms.Label ();
      this.RequestedAnalogGain = new System.Windows.Forms.NumericUpDown ();
      this.AnalogGainLabel = new System.Windows.Forms.Label ();
      this.NumSampleLines = new System.Windows.Forms.NumericUpDown ();
      this.NumSampleLinesLabel = new System.Windows.Forms.Label ();
      this.BottomControlsPannel = new System.Windows.Forms.Panel ();
      this.MediumSmootingWindowLabel = new System.Windows.Forms.Label ();
      this.MediumSmootingWindow = new System.Windows.Forms.NumericUpDown ();
      this.AutoGainButton = new System.Windows.Forms.Button ();
      this.AutoCropButton = new System.Windows.Forms.Button ();
      this.ScalingRationLabel = new System.Windows.Forms.Label ();
      this.ScalingRatio = new System.Windows.Forms.NumericUpDown ();
      ((System.ComponentModel.ISupportInitialize)(this.RequestedDigitalGain)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.RequestedAnalogGain)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.NumSampleLines)).BeginInit ();
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
      // refreshButton
      // 
      this.refreshButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.refreshButton.Location = new System.Drawing.Point (625, 19);
      this.refreshButton.Name = "refreshButton";
      this.refreshButton.Size = new System.Drawing.Size (77, 36);
      this.refreshButton.TabIndex = 260;
      this.refreshButton.Text = "Refresh";
      this.refreshButton.UseVisualStyleBackColor = true;
      this.refreshButton.Click += new System.EventHandler (this.refreshButton_Click_1);
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
      // AutoRefreshButton
      // 
      this.AutoRefreshButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AutoRefreshButton.Location = new System.Drawing.Point (715, 19);
      this.AutoRefreshButton.Name = "AutoRefreshButton";
      this.AutoRefreshButton.Size = new System.Drawing.Size (113, 36);
      this.AutoRefreshButton.TabIndex = 256;
      this.AutoRefreshButton.Text = "Auto Refresh";
      this.AutoRefreshButton.UseVisualStyleBackColor = true;
      this.AutoRefreshButton.Click += new System.EventHandler (this.AutoRefreshButton_Click);
      // 
      // autoRefreshTimer
      // 
      this.autoRefreshTimer.Tick += new System.EventHandler (this.autoRefreshTimer_Tick);
      // 
      // RequestedDigitalGain
      // 
      this.RequestedDigitalGain.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F);
      this.RequestedDigitalGain.Increment = new decimal (new int[] {
            100,
            0,
            0,
            0});
      this.RequestedDigitalGain.Location = new System.Drawing.Point (241, 30);
      this.RequestedDigitalGain.Maximum = new decimal (new int[] {
            5000,
            0,
            0,
            0});
      this.RequestedDigitalGain.Name = "RequestedDigitalGain";
      this.RequestedDigitalGain.Size = new System.Drawing.Size (71, 22);
      this.RequestedDigitalGain.TabIndex = 245;
      this.RequestedDigitalGain.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.RequestedDigitalGain.Value = new decimal (new int[] {
            10,
            0,
            0,
            65536});
      this.RequestedDigitalGain.ValueChanged += new System.EventHandler (this.RequestedDigitalGain_ValueChanged);
      // 
      // DigitalGainLabel
      // 
      this.DigitalGainLabel.AutoSize = true;
      this.DigitalGainLabel.Location = new System.Drawing.Point (237, 15);
      this.DigitalGainLabel.Name = "DigitalGainLabel";
      this.DigitalGainLabel.Size = new System.Drawing.Size (61, 13);
      this.DigitalGainLabel.TabIndex = 221;
      this.DigitalGainLabel.Text = "Digital Gain";
      this.DigitalGainLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // RequestedAnalogGain
      // 
      this.RequestedAnalogGain.DecimalPlaces = 1;
      this.RequestedAnalogGain.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F);
      this.RequestedAnalogGain.Increment = new decimal (new int[] {
            1,
            0,
            0,
            65536});
      this.RequestedAnalogGain.Location = new System.Drawing.Point (164, 30);
      this.RequestedAnalogGain.Maximum = new decimal (new int[] {
            10,
            0,
            0,
            0});
      this.RequestedAnalogGain.Minimum = new decimal (new int[] {
            10,
            0,
            0,
            -2147483648});
      this.RequestedAnalogGain.Name = "RequestedAnalogGain";
      this.RequestedAnalogGain.Size = new System.Drawing.Size (62, 22);
      this.RequestedAnalogGain.TabIndex = 240;
      this.RequestedAnalogGain.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.RequestedAnalogGain.Value = new decimal (new int[] {
            10,
            0,
            0,
            65536});
      this.RequestedAnalogGain.ValueChanged += new System.EventHandler (this.RequestedAnalogGain_ValueChanged);
      // 
      // AnalogGainLabel
      // 
      this.AnalogGainLabel.AutoSize = true;
      this.AnalogGainLabel.Location = new System.Drawing.Point (160, 15);
      this.AnalogGainLabel.Name = "AnalogGainLabel";
      this.AnalogGainLabel.Size = new System.Drawing.Size (65, 13);
      this.AnalogGainLabel.TabIndex = 219;
      this.AnalogGainLabel.Text = "Analog Gain";
      this.AnalogGainLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // NumSampleLines
      // 
      this.NumSampleLines.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F);
      this.NumSampleLines.Location = new System.Drawing.Point (86, 30);
      this.NumSampleLines.Maximum = new decimal (new int[] {
            20,
            0,
            0,
            0});
      this.NumSampleLines.Minimum = new decimal (new int[] {
            1,
            0,
            0,
            0});
      this.NumSampleLines.Name = "NumSampleLines";
      this.NumSampleLines.Size = new System.Drawing.Size (50, 22);
      this.NumSampleLines.TabIndex = 223;
      this.NumSampleLines.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.NumSampleLines.Value = new decimal (new int[] {
            1,
            0,
            0,
            0});
      this.NumSampleLines.ValueChanged += new System.EventHandler (this.NumSampleLines_ValueChanged);
      // 
      // NumSampleLinesLabel
      // 
      this.NumSampleLinesLabel.AutoSize = true;
      this.NumSampleLinesLabel.Location = new System.Drawing.Point (79, 14);
      this.NumSampleLinesLabel.Name = "NumSampleLinesLabel";
      this.NumSampleLinesLabel.Size = new System.Drawing.Size (57, 13);
      this.NumSampleLinesLabel.TabIndex = 224;
      this.NumSampleLinesLabel.Text = "# Samples";
      this.NumSampleLinesLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // BottomControlsPannel
      // 
      this.BottomControlsPannel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.BottomControlsPannel.Controls.Add (this.MediumSmootingWindowLabel);
      this.BottomControlsPannel.Controls.Add (this.MediumSmootingWindow);
      this.BottomControlsPannel.Controls.Add (this.AutoGainButton);
      this.BottomControlsPannel.Controls.Add (this.AutoCropButton);
      this.BottomControlsPannel.Controls.Add (this.ScalingRationLabel);
      this.BottomControlsPannel.Controls.Add (this.ScalingRatio);
      this.BottomControlsPannel.Controls.Add (this.AutoRefreshButton);
      this.BottomControlsPannel.Controls.Add (this.NumSampleLinesLabel);
      this.BottomControlsPannel.Controls.Add (this.exitButton);
      this.BottomControlsPannel.Controls.Add (this.RequestedDigitalGain);
      this.BottomControlsPannel.Controls.Add (this.NumSampleLines);
      this.BottomControlsPannel.Controls.Add (this.refreshButton);
      this.BottomControlsPannel.Controls.Add (this.AnalogGainLabel);
      this.BottomControlsPannel.Controls.Add (this.RequestedAnalogGain);
      this.BottomControlsPannel.Controls.Add (this.DigitalGainLabel);
      this.BottomControlsPannel.Location = new System.Drawing.Point (2, 484);
      this.BottomControlsPannel.Name = "BottomControlsPannel";
      this.BottomControlsPannel.Size = new System.Drawing.Size (964, 67);
      this.BottomControlsPannel.TabIndex = 225;
      // 
      // MediumSmootingWindowLabel
      // 
      this.MediumSmootingWindowLabel.AutoSize = true;
      this.MediumSmootingWindowLabel.Location = new System.Drawing.Point (323, 15);
      this.MediumSmootingWindowLabel.Name = "MediumSmootingWindowLabel";
      this.MediumSmootingWindowLabel.Size = new System.Drawing.Size (67, 13);
      this.MediumSmootingWindowLabel.TabIndex = 302;
      this.MediumSmootingWindowLabel.Text = "Med Smooth";
      this.MediumSmootingWindowLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // MediumSmootingWindow
      // 
      this.MediumSmootingWindow.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F);
      this.MediumSmootingWindow.Location = new System.Drawing.Point (326, 30);
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
      // AutoGainButton
      // 
      this.AutoGainButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AutoGainButton.Location = new System.Drawing.Point (510, 19);
      this.AutoGainButton.Name = "AutoGainButton";
      this.AutoGainButton.Size = new System.Drawing.Size (89, 36);
      this.AutoGainButton.TabIndex = 255;
      this.AutoGainButton.Text = "Auto Gain";
      this.AutoGainButton.UseVisualStyleBackColor = true;
      // 
      // AutoCropButton
      // 
      this.AutoCropButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AutoCropButton.Location = new System.Drawing.Point (409, 19);
      this.AutoCropButton.Name = "AutoCropButton";
      this.AutoCropButton.Size = new System.Drawing.Size (89, 36);
      this.AutoCropButton.TabIndex = 250;
      this.AutoCropButton.Text = "Auto Crop";
      this.AutoCropButton.UseVisualStyleBackColor = true;
      this.AutoCropButton.Click += new System.EventHandler (this.AutoCropButton_Click);
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
      ((System.ComponentModel.ISupportInitialize)(this.RequestedDigitalGain)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.RequestedAnalogGain)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.NumSampleLines)).EndInit ();
      this.BottomControlsPannel.ResumeLayout (false);
      this.BottomControlsPannel.PerformLayout ();
      ((System.ComponentModel.ISupportInitialize)(this.MediumSmootingWindow)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.ScalingRatio)).EndInit ();
      this.ResumeLayout (false);

    }

    #endregion

    private System.Windows.Forms.Button exitButton;
    private System.Windows.Forms.Button refreshButton;
    private System.Windows.Forms.HScrollBar pictureScrollBar;
    private System.Windows.Forms.Panel cameraLineWindow;
    private System.Windows.Forms.Button AutoRefreshButton;
    private System.Windows.Forms.Timer autoRefreshTimer;
    private System.Windows.Forms.NumericUpDown RequestedDigitalGain;
    private System.Windows.Forms.Label DigitalGainLabel;
    private System.Windows.Forms.NumericUpDown RequestedAnalogGain;
    private System.Windows.Forms.Label AnalogGainLabel;
    private System.Windows.Forms.NumericUpDown NumSampleLines;
    private System.Windows.Forms.Label NumSampleLinesLabel;
    private System.Windows.Forms.Panel BottomControlsPannel;
    private System.Windows.Forms.NumericUpDown ScalingRatio;
    private System.Windows.Forms.Label ScalingRationLabel;
    private System.Windows.Forms.Button AutoCropButton;
    private System.Windows.Forms.Button AutoGainButton;
    private System.Windows.Forms.Label MediumSmootingWindowLabel;
    private System.Windows.Forms.NumericUpDown MediumSmootingWindow;
  }
}