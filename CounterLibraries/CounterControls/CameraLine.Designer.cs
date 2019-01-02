namespace CounterControls
{
  partial class CameraLine
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

    #region Component Designer generated code

    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent ()
    {
      this.components = new System.ComponentModel.Container ();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager (typeof (CameraLine));
      this.BottomControlsPannel = new System.Windows.Forms.Panel ();
      this.RequestedSensitivityModeCB = new System.Windows.Forms.ComboBox ();
      this.SensitivityModeLabel = new System.Windows.Forms.Label ();
      this.MediumSmootingWindowLabel = new System.Windows.Forms.Label ();
      this.MediumSmootingWindow = new System.Windows.Forms.NumericUpDown ();
      this.AutoGainButton = new System.Windows.Forms.Button ();
      this.AutoCropButton = new System.Windows.Forms.Button ();
      this.ScalingRationLabel = new System.Windows.Forms.Label ();
      this.ScalingRatio = new System.Windows.Forms.NumericUpDown ();
      this.NumSampleLinesLabel = new System.Windows.Forms.Label ();
      this.RequestedDigitalGain = new System.Windows.Forms.NumericUpDown ();
      this.NumSampleLines = new System.Windows.Forms.NumericUpDown ();
      this.AnalogGainLabel = new System.Windows.Forms.Label ();
      this.RequestedAnalogGain = new System.Windows.Forms.NumericUpDown ();
      this.DigitalGainLabel = new System.Windows.Forms.Label ();
      this.pictureScrollBar = new System.Windows.Forms.HScrollBar ();
      this.cameraLineWindow = new System.Windows.Forms.Panel ();
      this.autoRefreshTimer = new System.Windows.Forms.Timer (this.components);
      this.cameraLineToolTip = new System.Windows.Forms.ToolTip (this.components);
      this.BottomControlsPannel.SuspendLayout ();
      ((System.ComponentModel.ISupportInitialize)(this.MediumSmootingWindow)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.ScalingRatio)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.RequestedDigitalGain)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.NumSampleLines)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.RequestedAnalogGain)).BeginInit ();
      this.SuspendLayout ();
      // 
      // BottomControlsPannel
      // 
      this.BottomControlsPannel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.BottomControlsPannel.Controls.Add (this.RequestedSensitivityModeCB);
      this.BottomControlsPannel.Controls.Add (this.SensitivityModeLabel);
      this.BottomControlsPannel.Controls.Add (this.MediumSmootingWindowLabel);
      this.BottomControlsPannel.Controls.Add (this.MediumSmootingWindow);
      this.BottomControlsPannel.Controls.Add (this.AutoGainButton);
      this.BottomControlsPannel.Controls.Add (this.AutoCropButton);
      this.BottomControlsPannel.Controls.Add (this.ScalingRationLabel);
      this.BottomControlsPannel.Controls.Add (this.ScalingRatio);
      this.BottomControlsPannel.Controls.Add (this.NumSampleLinesLabel);
      this.BottomControlsPannel.Controls.Add (this.RequestedDigitalGain);
      this.BottomControlsPannel.Controls.Add (this.NumSampleLines);
      this.BottomControlsPannel.Controls.Add (this.AnalogGainLabel);
      this.BottomControlsPannel.Controls.Add (this.RequestedAnalogGain);
      this.BottomControlsPannel.Controls.Add (this.DigitalGainLabel);
      this.BottomControlsPannel.Location = new System.Drawing.Point (3, 372);
      this.BottomControlsPannel.Name = "BottomControlsPannel";
      this.BottomControlsPannel.Size = new System.Drawing.Size (837, 75);
      this.BottomControlsPannel.TabIndex = 227;
      // 
      // RequestedSensitivityModeCB
      // 
      this.RequestedSensitivityModeCB.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.RequestedSensitivityModeCB.FormattingEnabled = true;
      this.RequestedSensitivityModeCB.Items.AddRange (new object[] {
            "Low",
            "High",
            "Tall"});
      this.RequestedSensitivityModeCB.Location = new System.Drawing.Point (302, 28);
      this.RequestedSensitivityModeCB.MaxDropDownItems = 3;
      this.RequestedSensitivityModeCB.Name = "RequestedSensitivityModeCB";
      this.RequestedSensitivityModeCB.Size = new System.Drawing.Size (68, 28);
      this.RequestedSensitivityModeCB.TabIndex = 130;
      this.RequestedSensitivityModeCB.Text = "Low";
      this.cameraLineToolTip.SetToolTip (this.RequestedSensitivityModeCB, resources.GetString ("RequestedSensitivityModeCB.ToolTip"));
      this.RequestedSensitivityModeCB.SelectedIndexChanged += new System.EventHandler (this.RequestedSensitivityModeCB_SelectedIndexChanged);
      // 
      // SensitivityModeLabel
      // 
      this.SensitivityModeLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SensitivityModeLabel.Location = new System.Drawing.Point (301, 10);
      this.SensitivityModeLabel.Name = "SensitivityModeLabel";
      this.SensitivityModeLabel.Size = new System.Drawing.Size (73, 18);
      this.SensitivityModeLabel.TabIndex = 303;
      this.SensitivityModeLabel.Text = "Sensitivity";
      this.SensitivityModeLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // MediumSmootingWindowLabel
      // 
      this.MediumSmootingWindowLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.MediumSmootingWindowLabel.Location = new System.Drawing.Point (170, 10);
      this.MediumSmootingWindowLabel.Name = "MediumSmootingWindowLabel";
      this.MediumSmootingWindowLabel.Size = new System.Drawing.Size (87, 18);
      this.MediumSmootingWindowLabel.TabIndex = 302;
      this.MediumSmootingWindowLabel.Text = "Med Smooth";
      this.MediumSmootingWindowLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // MediumSmootingWindow
      // 
      this.MediumSmootingWindow.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.MediumSmootingWindow.Location = new System.Drawing.Point (179, 30);
      this.MediumSmootingWindow.Maximum = new decimal (new int[] {
            50,
            0,
            0,
            0});
      this.MediumSmootingWindow.Name = "MediumSmootingWindow";
      this.MediumSmootingWindow.Size = new System.Drawing.Size (67, 26);
      this.MediumSmootingWindow.TabIndex = 120;
      this.MediumSmootingWindow.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.cameraLineToolTip.SetToolTip (this.MediumSmootingWindow, "The number of pixels to the left and right of each scan-line point to use for med" +
              "ium \\\r\nsmoothing; a value of ‘0’ indicates no smoothing.");
      // 
      // AutoGainButton
      // 
      this.AutoGainButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AutoGainButton.Location = new System.Drawing.Point (740, 4);
      this.AutoGainButton.Name = "AutoGainButton";
      this.AutoGainButton.Size = new System.Drawing.Size (80, 65);
      this.AutoGainButton.TabIndex = 210;
      this.AutoGainButton.Text = "Auto Gain";
      this.cameraLineToolTip.SetToolTip (this.AutoGainButton, "Press this button to have Larcos automatically set the gain settings.  Once this " +
              "procedure is started it \r\nwill take several seconds to complete.");
      this.AutoGainButton.UseVisualStyleBackColor = true;
      this.AutoGainButton.Click += new System.EventHandler (this.AutoGainButton_Click);
      // 
      // AutoCropButton
      // 
      this.AutoCropButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AutoCropButton.Location = new System.Drawing.Point (640, 4);
      this.AutoCropButton.Name = "AutoCropButton";
      this.AutoCropButton.Size = new System.Drawing.Size (80, 65);
      this.AutoCropButton.TabIndex = 200;
      this.AutoCropButton.Text = "Auto Crop";
      this.cameraLineToolTip.SetToolTip (this.AutoCropButton, "Press this button to automatically set the left/right crop settings.  You will al" +
              "so be able to manual \r\nset these values in the “Video” tab.");
      this.AutoCropButton.UseVisualStyleBackColor = true;
      this.AutoCropButton.Click += new System.EventHandler (this.AutoCropButton_Click);
      // 
      // ScalingRationLabel
      // 
      this.ScalingRationLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ScalingRationLabel.Location = new System.Drawing.Point (18, 10);
      this.ScalingRationLabel.Name = "ScalingRationLabel";
      this.ScalingRationLabel.Size = new System.Drawing.Size (59, 18);
      this.ScalingRationLabel.TabIndex = 226;
      this.ScalingRationLabel.Text = "Scaling";
      this.ScalingRationLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // ScalingRatio
      // 
      this.ScalingRatio.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
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
      this.ScalingRatio.Size = new System.Drawing.Size (60, 26);
      this.ScalingRatio.TabIndex = 100;
      this.ScalingRatio.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.cameraLineToolTip.SetToolTip (this.ScalingRatio, resources.GetString ("ScalingRatio.ToolTip"));
      this.ScalingRatio.Value = new decimal (new int[] {
            1,
            0,
            0,
            0});
      this.ScalingRatio.ValueChanged += new System.EventHandler (this.ScalingRatio_ValueChanged);
      // 
      // NumSampleLinesLabel
      // 
      this.NumSampleLinesLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.NumSampleLinesLabel.Location = new System.Drawing.Point (90, 10);
      this.NumSampleLinesLabel.Name = "NumSampleLinesLabel";
      this.NumSampleLinesLabel.Size = new System.Drawing.Size (73, 18);
      this.NumSampleLinesLabel.TabIndex = 224;
      this.NumSampleLinesLabel.Text = "# Samples";
      this.NumSampleLinesLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // RequestedDigitalGain
      // 
      this.RequestedDigitalGain.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.RequestedDigitalGain.Increment = new decimal (new int[] {
            100,
            0,
            0,
            0});
      this.RequestedDigitalGain.Location = new System.Drawing.Point (484, 30);
      this.RequestedDigitalGain.Maximum = new decimal (new int[] {
            20000,
            0,
            0,
            0});
      this.RequestedDigitalGain.Name = "RequestedDigitalGain";
      this.RequestedDigitalGain.Size = new System.Drawing.Size (76, 26);
      this.RequestedDigitalGain.TabIndex = 150;
      this.RequestedDigitalGain.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.cameraLineToolTip.SetToolTip (this.RequestedDigitalGain, "This is the gain applied after the Analog-to-Digital conversion; you would typica" +
              "lly tune \r\nthis value after you set the Analog-Gain value.  Not all cameras have" +
              " this parameter.");
      this.RequestedDigitalGain.Value = new decimal (new int[] {
            10,
            0,
            0,
            65536});
      this.RequestedDigitalGain.ValueChanged += new System.EventHandler (this.RequestedDigitalGain_ValueChanged);
      // 
      // NumSampleLines
      // 
      this.NumSampleLines.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.NumSampleLines.Location = new System.Drawing.Point (101, 30);
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
      this.NumSampleLines.Size = new System.Drawing.Size (55, 26);
      this.NumSampleLines.TabIndex = 110;
      this.NumSampleLines.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.cameraLineToolTip.SetToolTip (this.NumSampleLines, "The number of scan lines to average together; each sample line comes from the 1st" +
              " scan line in a frame.");
      this.NumSampleLines.Value = new decimal (new int[] {
            1,
            0,
            0,
            0});
      this.NumSampleLines.ValueChanged += new System.EventHandler (this.NumSampleLines_ValueChanged);
      // 
      // AnalogGainLabel
      // 
      this.AnalogGainLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AnalogGainLabel.Location = new System.Drawing.Point (386, 10);
      this.AnalogGainLabel.Name = "AnalogGainLabel";
      this.AnalogGainLabel.Size = new System.Drawing.Size (83, 18);
      this.AnalogGainLabel.TabIndex = 219;
      this.AnalogGainLabel.Text = "Analog Gain";
      this.AnalogGainLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // RequestedAnalogGain
      // 
      this.RequestedAnalogGain.DecimalPlaces = 1;
      this.RequestedAnalogGain.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.RequestedAnalogGain.Increment = new decimal (new int[] {
            1,
            0,
            0,
            65536});
      this.RequestedAnalogGain.Location = new System.Drawing.Point (392, 30);
      this.RequestedAnalogGain.Maximum = new decimal (new int[] {
            1000,
            0,
            0,
            0});
      this.RequestedAnalogGain.Minimum = new decimal (new int[] {
            1000,
            0,
            0,
            -2147483648});
      this.RequestedAnalogGain.Name = "RequestedAnalogGain";
      this.RequestedAnalogGain.Size = new System.Drawing.Size (67, 26);
      this.RequestedAnalogGain.TabIndex = 140;
      this.RequestedAnalogGain.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.cameraLineToolTip.SetToolTip (this.RequestedAnalogGain, resources.GetString ("RequestedAnalogGain.ToolTip"));
      this.RequestedAnalogGain.Value = new decimal (new int[] {
            10,
            0,
            0,
            65536});
      this.RequestedAnalogGain.ValueChanged += new System.EventHandler (this.RequestedAnalogGain_ValueChanged);
      // 
      // DigitalGainLabel
      // 
      this.DigitalGainLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.DigitalGainLabel.Location = new System.Drawing.Point (481, 10);
      this.DigitalGainLabel.Name = "DigitalGainLabel";
      this.DigitalGainLabel.Size = new System.Drawing.Size (78, 18);
      this.DigitalGainLabel.TabIndex = 221;
      this.DigitalGainLabel.Text = "Digital Gain";
      this.DigitalGainLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // pictureScrollBar
      // 
      this.pictureScrollBar.LargeChange = 512;
      this.pictureScrollBar.Location = new System.Drawing.Point (0, 346);
      this.pictureScrollBar.Maximum = 4096;
      this.pictureScrollBar.Name = "pictureScrollBar";
      this.pictureScrollBar.Size = new System.Drawing.Size (840, 22);
      this.pictureScrollBar.TabIndex = 229;
      this.pictureScrollBar.ValueChanged += new System.EventHandler (this.pictureScrollBar_ValueChanged_1);
      // 
      // cameraLineWindow
      // 
      this.cameraLineWindow.BackColor = System.Drawing.SystemColors.WindowText;
      this.cameraLineWindow.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.cameraLineWindow.Font = new System.Drawing.Font ("Arial", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.cameraLineWindow.Location = new System.Drawing.Point (2, 2);
      this.cameraLineWindow.Name = "cameraLineWindow";
      this.cameraLineWindow.Size = new System.Drawing.Size (840, 342);
      this.cameraLineWindow.TabIndex = 230;
      this.cameraLineWindow.Paint += new System.Windows.Forms.PaintEventHandler (this.cameraLineWindow_Paint);
      // 
      // autoRefreshTimer
      // 
      this.autoRefreshTimer.Enabled = true;
      this.autoRefreshTimer.Tick += new System.EventHandler (this.autoRefreshTimer_Tick);
      // 
      // cameraLineToolTip
      // 
      this.cameraLineToolTip.AutoPopDelay = 7500;
      this.cameraLineToolTip.InitialDelay = 250;
      this.cameraLineToolTip.ReshowDelay = 100;
      // 
      // CameraLine
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add (this.cameraLineWindow);
      this.Controls.Add (this.pictureScrollBar);
      this.Controls.Add (this.BottomControlsPannel);
      this.Font = new System.Drawing.Font ("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Name = "CameraLine";
      this.Size = new System.Drawing.Size (848, 450);
      this.Load += new System.EventHandler (this.CameraLine_Load);
      this.VisibleChanged += new System.EventHandler (this.CameraLine_VisibleChanged);
      this.BottomControlsPannel.ResumeLayout (false);
      ((System.ComponentModel.ISupportInitialize)(this.MediumSmootingWindow)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.ScalingRatio)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.RequestedDigitalGain)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.NumSampleLines)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.RequestedAnalogGain)).EndInit ();
      this.ResumeLayout (false);

    }

    #endregion

    private System.Windows.Forms.Panel BottomControlsPannel;
    private System.Windows.Forms.Label MediumSmootingWindowLabel;
    private System.Windows.Forms.NumericUpDown MediumSmootingWindow;
    private System.Windows.Forms.Button AutoGainButton;
    private System.Windows.Forms.Button AutoCropButton;
    private System.Windows.Forms.Label ScalingRationLabel;
    private System.Windows.Forms.NumericUpDown ScalingRatio;
    private System.Windows.Forms.Label NumSampleLinesLabel;
    private System.Windows.Forms.NumericUpDown RequestedDigitalGain;
    private System.Windows.Forms.NumericUpDown NumSampleLines;
    private System.Windows.Forms.Label AnalogGainLabel;
    private System.Windows.Forms.NumericUpDown RequestedAnalogGain;
    private System.Windows.Forms.Label DigitalGainLabel;
    private System.Windows.Forms.HScrollBar pictureScrollBar;
    private System.Windows.Forms.Panel cameraLineWindow;
    private System.Windows.Forms.Timer autoRefreshTimer;
    private System.Windows.Forms.ComboBox RequestedSensitivityModeCB;
    private System.Windows.Forms.Label SensitivityModeLabel;
    private System.Windows.Forms.ToolTip cameraLineToolTip;
  }
}
