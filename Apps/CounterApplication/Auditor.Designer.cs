namespace CounterApplication
{
  partial class Auditor
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
      System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea ();
      System.Windows.Forms.DataVisualization.Charting.StripLine stripLine1 = new System.Windows.Forms.DataVisualization.Charting.StripLine ();
      System.Windows.Forms.DataVisualization.Charting.StripLine stripLine2 = new System.Windows.Forms.DataVisualization.Charting.StripLine ();
      System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend ();
      System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series ();
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint1 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (0.0069444444444444441, 100);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint2 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (0.013888888888888888, 970);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint3 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (0.021527777777777778, 103);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint4 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (0.045277777777777778, 0);
      this.ScannerFileChart = new System.Windows.Forms.DataVisualization.Charting.Chart ();
      this.CountLabel = new System.Windows.Forms.Label ();
      this.Count = new System.Windows.Forms.TextBox ();
      this.ScannerFileLenInSecsLabel = new System.Windows.Forms.Label ();
      this.ScannerFileLenInSecs = new System.Windows.Forms.TextBox ();
      this.DescriptionLabel = new System.Windows.Forms.Label ();
      this.Description = new System.Windows.Forms.TextBox ();
      this.ScannerFileScrollBar = new System.Windows.Forms.HScrollBar ();
      this.PlotOptions = new System.Windows.Forms.ComboBox ();
      this.PlotOptionsLabel = new System.Windows.Forms.Label ();
      this.AuditorTimer = new System.Windows.Forms.Timer (this.components);
      this.ScannerFileLenInScanLines = new System.Windows.Forms.TextBox ();
      this.ScannerFileLenInScanLinesLabel = new System.Windows.Forms.Label ();
      this.AutoScrollingRate = new System.Windows.Forms.TextBox ();
      this.ScannerDisplayControls = new System.Windows.Forms.Panel ();
      this.SkipToPrevButton = new System.Windows.Forms.Button ();
      this.SkipToNextButton = new System.Windows.Forms.Button ();
      this.Colorize = new System.Windows.Forms.CheckBox ();
      this.StopAutoScrol = new System.Windows.Forms.Button ();
      this.FastForwardButton = new System.Windows.Forms.Button ();
      this.FastBackwardButon = new System.Windows.Forms.Button ();
      this.GenerateReportButton = new System.Windows.Forms.Button ();
      this.RunRecountButton = new System.Windows.Forms.Button ();
      this.auditorToolTip = new System.Windows.Forms.ToolTip (this.components);
      this.ValidateByClassButton = new System.Windows.Forms.Button ();
      this.ScannerFilePanel = new CounterControls.ScannerFileViewerPanel ();
      ((System.ComponentModel.ISupportInitialize)(this.ScannerFileChart)).BeginInit ();
      this.ScannerDisplayControls.SuspendLayout ();
      this.SuspendLayout ();
      // 
      // ScannerFileChart
      // 
      this.ScannerFileChart.BorderlineColor = System.Drawing.Color.Black;
      this.ScannerFileChart.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      chartArea1.AxisX.IntervalAutoMode = System.Windows.Forms.DataVisualization.Charting.IntervalAutoMode.VariableCount;
      chartArea1.AxisX.IsLabelAutoFit = false;
      chartArea1.AxisX.LabelStyle.Format = "mm:ss";
      chartArea1.AxisX.MajorGrid.LineColor = System.Drawing.Color.Gray;
      chartArea1.AxisX.MajorGrid.LineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Dash;
      stripLine1.BackColor = System.Drawing.Color.Red;
      stripLine1.BorderColor = System.Drawing.Color.Red;
      stripLine1.BorderWidth = 3;
      stripLine1.ForeColor = System.Drawing.Color.Empty;
      stripLine1.Interval = 100000;
      stripLine1.IntervalOffset = 600;
      stripLine1.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Seconds;
      stripLine1.StripWidthType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Seconds;
      stripLine2.BackColor = System.Drawing.Color.FromArgb (((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
      stripLine2.BorderColor = System.Drawing.Color.FromArgb (((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
      stripLine2.BorderWidth = 3;
      stripLine2.Interval = 100000;
      stripLine2.IntervalOffset = 43;
      stripLine2.IntervalOffsetType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Minutes;
      stripLine2.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Minutes;
      stripLine2.StripWidthType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Minutes;
      chartArea1.AxisX.StripLines.Add (stripLine1);
      chartArea1.AxisX.StripLines.Add (stripLine2);
      chartArea1.AxisY.IntervalAutoMode = System.Windows.Forms.DataVisualization.Charting.IntervalAutoMode.VariableCount;
      chartArea1.AxisY.LabelAutoFitMinFontSize = 8;
      chartArea1.AxisY.MajorGrid.Interval = 0;
      chartArea1.AxisY.MajorGrid.LineColor = System.Drawing.Color.Silver;
      chartArea1.AxisY.MajorGrid.LineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Dash;
      chartArea1.AxisY.MajorTickMark.Interval = 0;
      chartArea1.AxisY2.LabelStyle.Enabled = false;
      chartArea1.AxisY2.MajorGrid.Enabled = false;
      chartArea1.AxisY2.MajorTickMark.Enabled = false;
      chartArea1.Name = "ChartArea1";
      chartArea1.Position.Auto = false;
      chartArea1.Position.Height = 94F;
      chartArea1.Position.Width = 98F;
      chartArea1.Position.Y = 2F;
      this.ScannerFileChart.ChartAreas.Add (chartArea1);
      legend1.Enabled = false;
      legend1.Name = "Legend1";
      this.ScannerFileChart.Legends.Add (legend1);
      this.ScannerFileChart.Location = new System.Drawing.Point (10, 67);
      this.ScannerFileChart.Name = "ScannerFileChart";
      series1.ChartArea = "ChartArea1";
      series1.Legend = "Legend1";
      series1.Name = "Series1";
      dataPoint3.LabelFormat = "\'MM:SS\'";
      series1.Points.Add (dataPoint1);
      series1.Points.Add (dataPoint2);
      series1.Points.Add (dataPoint3);
      series1.Points.Add (dataPoint4);
      series1.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Time;
      this.ScannerFileChart.Series.Add (series1);
      this.ScannerFileChart.Size = new System.Drawing.Size (962, 112);
      this.ScannerFileChart.TabIndex = 1;
      this.ScannerFileChart.Text = "chart1";
      this.ScannerFileChart.MouseClick += new System.Windows.Forms.MouseEventHandler (this.ScannerFileChart_MouseClick);
      // 
      // CountLabel
      // 
      this.CountLabel.AutoSize = true;
      this.CountLabel.Location = new System.Drawing.Point (10, 9);
      this.CountLabel.Name = "CountLabel";
      this.CountLabel.Size = new System.Drawing.Size (35, 13);
      this.CountLabel.TabIndex = 2;
      this.CountLabel.Text = "Count";
      // 
      // Count
      // 
      this.Count.Font = new System.Drawing.Font ("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Count.Location = new System.Drawing.Point (10, 25);
      this.Count.Name = "Count";
      this.Count.ReadOnly = true;
      this.Count.Size = new System.Drawing.Size (108, 29);
      this.Count.TabIndex = 3;
      this.Count.Text = "1,234,567";
      this.Count.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // ScannerFileLenInSecsLabel
      // 
      this.ScannerFileLenInSecsLabel.AutoSize = true;
      this.ScannerFileLenInSecsLabel.Location = new System.Drawing.Point (138, 11);
      this.ScannerFileLenInSecsLabel.Name = "ScannerFileLenInSecsLabel";
      this.ScannerFileLenInSecsLabel.Size = new System.Drawing.Size (58, 13);
      this.ScannerFileLenInSecsLabel.TabIndex = 4;
      this.ScannerFileLenInSecsLabel.Text = "Time Span";
      // 
      // ScannerFileLenInSecs
      // 
      this.ScannerFileLenInSecs.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ScannerFileLenInSecs.Location = new System.Drawing.Point (128, 27);
      this.ScannerFileLenInSecs.Name = "ScannerFileLenInSecs";
      this.ScannerFileLenInSecs.ReadOnly = true;
      this.ScannerFileLenInSecs.Size = new System.Drawing.Size (77, 26);
      this.ScannerFileLenInSecs.TabIndex = 5;
      this.ScannerFileLenInSecs.Text = "10:12";
      this.ScannerFileLenInSecs.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // DescriptionLabel
      // 
      this.DescriptionLabel.AutoSize = true;
      this.DescriptionLabel.Location = new System.Drawing.Point (494, 9);
      this.DescriptionLabel.Name = "DescriptionLabel";
      this.DescriptionLabel.Size = new System.Drawing.Size (60, 13);
      this.DescriptionLabel.TabIndex = 6;
      this.DescriptionLabel.Text = "Description";
      // 
      // Description
      // 
      this.Description.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Description.Location = new System.Drawing.Point (497, 25);
      this.Description.Multiline = true;
      this.Description.Name = "Description";
      this.Description.ReadOnly = true;
      this.Description.Size = new System.Drawing.Size (474, 36);
      this.Description.TabIndex = 7;
      this.Description.Text = "Description Of Data Collection";
      // 
      // ScannerFileScrollBar
      // 
      this.ScannerFileScrollBar.Location = new System.Drawing.Point (10, 679);
      this.ScannerFileScrollBar.Name = "ScannerFileScrollBar";
      this.ScannerFileScrollBar.Padding = new System.Windows.Forms.Padding (1);
      this.ScannerFileScrollBar.Size = new System.Drawing.Size (961, 22);
      this.ScannerFileScrollBar.TabIndex = 8;
      this.ScannerFileScrollBar.Scroll += new System.Windows.Forms.ScrollEventHandler (this.ScannerFileScrollBar_Scroll);
      // 
      // PlotOptions
      // 
      this.PlotOptions.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.PlotOptions.FormattingEnabled = true;
      this.PlotOptions.Location = new System.Drawing.Point (344, 27);
      this.PlotOptions.Name = "PlotOptions";
      this.PlotOptions.Size = new System.Drawing.Size (147, 28);
      this.PlotOptions.TabIndex = 9;
      this.PlotOptions.SelectedIndexChanged += new System.EventHandler (this.PlotOptions_SelectedIndexChanged);
      // 
      // PlotOptionsLabel
      // 
      this.PlotOptionsLabel.AutoSize = true;
      this.PlotOptionsLabel.Location = new System.Drawing.Point (341, 11);
      this.PlotOptionsLabel.Name = "PlotOptionsLabel";
      this.PlotOptionsLabel.Size = new System.Drawing.Size (64, 13);
      this.PlotOptionsLabel.TabIndex = 10;
      this.PlotOptionsLabel.Text = "Plot Options";
      // 
      // AuditorTimer
      // 
      this.AuditorTimer.Enabled = true;
      this.AuditorTimer.Tick += new System.EventHandler (this.AuditorTimer_Tick);
      // 
      // ScannerFileLenInScanLines
      // 
      this.ScannerFileLenInScanLines.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ScannerFileLenInScanLines.Location = new System.Drawing.Point (218, 27);
      this.ScannerFileLenInScanLines.Name = "ScannerFileLenInScanLines";
      this.ScannerFileLenInScanLines.ReadOnly = true;
      this.ScannerFileLenInScanLines.Size = new System.Drawing.Size (105, 26);
      this.ScannerFileLenInScanLines.TabIndex = 12;
      this.ScannerFileLenInScanLines.Text = "78,134,123";
      this.ScannerFileLenInScanLines.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // ScannerFileLenInScanLinesLabel
      // 
      this.ScannerFileLenInScanLinesLabel.AutoSize = true;
      this.ScannerFileLenInScanLinesLabel.Location = new System.Drawing.Point (239, 11);
      this.ScannerFileLenInScanLinesLabel.Name = "ScannerFileLenInScanLinesLabel";
      this.ScannerFileLenInScanLinesLabel.Size = new System.Drawing.Size (57, 13);
      this.ScannerFileLenInScanLinesLabel.TabIndex = 13;
      this.ScannerFileLenInScanLinesLabel.Text = "ScanLines";
      // 
      // AutoScrollingRate
      // 
      this.AutoScrollingRate.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AutoScrollingRate.Location = new System.Drawing.Point (304, 14);
      this.AutoScrollingRate.Name = "AutoScrollingRate";
      this.AutoScrollingRate.ReadOnly = true;
      this.AutoScrollingRate.Size = new System.Drawing.Size (55, 26);
      this.AutoScrollingRate.TabIndex = 14;
      this.AutoScrollingRate.Text = "0";
      this.AutoScrollingRate.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // ScannerDisplayControls
      // 
      this.ScannerDisplayControls.Controls.Add (this.SkipToPrevButton);
      this.ScannerDisplayControls.Controls.Add (this.SkipToNextButton);
      this.ScannerDisplayControls.Controls.Add (this.Colorize);
      this.ScannerDisplayControls.Controls.Add (this.StopAutoScrol);
      this.ScannerDisplayControls.Controls.Add (this.FastForwardButton);
      this.ScannerDisplayControls.Controls.Add (this.FastBackwardButon);
      this.ScannerDisplayControls.Controls.Add (this.AutoScrollingRate);
      this.ScannerDisplayControls.Location = new System.Drawing.Point (204, 706);
      this.ScannerDisplayControls.Name = "ScannerDisplayControls";
      this.ScannerDisplayControls.Size = new System.Drawing.Size (469, 51);
      this.ScannerDisplayControls.TabIndex = 17;
      // 
      // SkipToPrevButton
      // 
      this.SkipToPrevButton.Image = global::CounterApplication.Properties.Resources.SkipToPrev;
      this.SkipToPrevButton.Location = new System.Drawing.Point (9, 4);
      this.SkipToPrevButton.Name = "SkipToPrevButton";
      this.SkipToPrevButton.Size = new System.Drawing.Size (51, 45);
      this.SkipToPrevButton.TabIndex = 20;
      this.SkipToPrevButton.UseVisualStyleBackColor = true;
      this.SkipToPrevButton.Click += new System.EventHandler (this.SkipToPrevButton_Click);
      // 
      // SkipToNextButton
      // 
      this.SkipToNextButton.Image = global::CounterApplication.Properties.Resources.SkipToNext;
      this.SkipToNextButton.Location = new System.Drawing.Point (237, 4);
      this.SkipToNextButton.Name = "SkipToNextButton";
      this.SkipToNextButton.Size = new System.Drawing.Size (51, 45);
      this.SkipToNextButton.TabIndex = 19;
      this.SkipToNextButton.UseVisualStyleBackColor = true;
      this.SkipToNextButton.Click += new System.EventHandler (this.SkipToNextButton_Click);
      // 
      // Colorize
      // 
      this.Colorize.AutoSize = true;
      this.Colorize.Location = new System.Drawing.Point (391, 19);
      this.Colorize.Name = "Colorize";
      this.Colorize.Size = new System.Drawing.Size (63, 17);
      this.Colorize.TabIndex = 18;
      this.Colorize.Text = "Colorize";
      this.auditorToolTip.SetToolTip (this.Colorize, "When checked will assign differemt colors to teh different gray level values.");
      this.Colorize.UseVisualStyleBackColor = true;
      this.Colorize.CheckedChanged += new System.EventHandler (this.Colorize_CheckedChanged);
      // 
      // StopAutoScrol
      // 
      this.StopAutoScrol.Image = global::CounterApplication.Properties.Resources.Stop;
      this.StopAutoScrol.Location = new System.Drawing.Point (123, 4);
      this.StopAutoScrol.Name = "StopAutoScrol";
      this.StopAutoScrol.Size = new System.Drawing.Size (51, 45);
      this.StopAutoScrol.TabIndex = 17;
      this.StopAutoScrol.UseVisualStyleBackColor = true;
      this.StopAutoScrol.Click += new System.EventHandler (this.StopAutoScrol_Click);
      // 
      // FastForwardButton
      // 
      this.FastForwardButton.Image = global::CounterApplication.Properties.Resources.GoForward1;
      this.FastForwardButton.Location = new System.Drawing.Point (180, 4);
      this.FastForwardButton.Name = "FastForwardButton";
      this.FastForwardButton.Size = new System.Drawing.Size (51, 45);
      this.FastForwardButton.TabIndex = 15;
      this.FastForwardButton.UseVisualStyleBackColor = true;
      this.FastForwardButton.Click += new System.EventHandler (this.FastForwardButton_Click);
      // 
      // FastBackwardButon
      // 
      this.FastBackwardButon.Image = global::CounterApplication.Properties.Resources.GoBackward;
      this.FastBackwardButon.Location = new System.Drawing.Point (66, 4);
      this.FastBackwardButon.Name = "FastBackwardButon";
      this.FastBackwardButon.Size = new System.Drawing.Size (51, 45);
      this.FastBackwardButon.TabIndex = 16;
      this.FastBackwardButon.UseVisualStyleBackColor = true;
      this.FastBackwardButon.Click += new System.EventHandler (this.FastBackwardButon_Click);
      // 
      // GenerateReportButton
      // 
      this.GenerateReportButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.GenerateReportButton.Location = new System.Drawing.Point (829, 712);
      this.GenerateReportButton.Name = "GenerateReportButton";
      this.GenerateReportButton.Size = new System.Drawing.Size (140, 37);
      this.GenerateReportButton.TabIndex = 18;
      this.GenerateReportButton.Text = "Generate Report";
      this.auditorToolTip.SetToolTip (this.GenerateReportButton, "If you accept the count then press this button to produce a Finale Report.");
      this.GenerateReportButton.UseVisualStyleBackColor = true;
      this.GenerateReportButton.Click += new System.EventHandler (this.GenerateReportButton_Click);
      // 
      // RunRecountButton
      // 
      this.RunRecountButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.RunRecountButton.Location = new System.Drawing.Point (9, 712);
      this.RunRecountButton.Name = "RunRecountButton";
      this.RunRecountButton.Size = new System.Drawing.Size (140, 37);
      this.RunRecountButton.TabIndex = 19;
      this.RunRecountButton.Text = "Run Recount";
      this.auditorToolTip.SetToolTip (this.RunRecountButton, "Press this button to replay this Scanner File;  meaning \r\nperform a recount.");
      this.RunRecountButton.UseVisualStyleBackColor = true;
      this.RunRecountButton.Click += new System.EventHandler (this.RunRecountButton_Click);
      // 
      // auditorToolTip
      // 
      this.auditorToolTip.AutoPopDelay = 7500;
      this.auditorToolTip.InitialDelay = 250;
      this.auditorToolTip.ReshowDelay = 100;
      // 
      // ValidateByClassButton
      // 
      this.ValidateByClassButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ValidateByClassButton.Location = new System.Drawing.Point (712, 712);
      this.ValidateByClassButton.Name = "ValidateByClassButton";
      this.ValidateByClassButton.Size = new System.Drawing.Size (88, 37);
      this.ValidateByClassButton.TabIndex = 20;
      this.ValidateByClassButton.Text = "By Class";
      this.auditorToolTip.SetToolTip (this.ValidateByClassButton, "Press this button to replay this Scanner File;  meaning \r\nperform a recount.");
      this.ValidateByClassButton.UseVisualStyleBackColor = true;
      this.ValidateByClassButton.Click += new System.EventHandler (this.ValidateByClassButton_Click);
      // 
      // ScannerFilePanel
      // 
      this.ScannerFilePanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.ScannerFilePanel.Colorize = false;
      this.ScannerFilePanel.Location = new System.Drawing.Point (12, 186);
      this.ScannerFilePanel.Name = "ScannerFilePanel";
      this.ScannerFilePanel.Size = new System.Drawing.Size (959, 491);
      this.ScannerFilePanel.TabIndex = 0;
      this.ScannerFilePanel.CropSettingsChanged += new CounterControls.ScannerFileViewerPanel.CropSettingsChangedHandler (this.ScannerFilePanel_CropSettingsChanged);
      this.ScannerFilePanel.Load += new System.EventHandler (this.ScannerFilePanel_Load);
      this.ScannerFilePanel.ParticleSelected += new CounterControls.ScannerFileViewerPanel.ParticleSelectedHandler (this.ScannerFilePanel_ParticleSelected);
      this.ScannerFilePanel.StartStopPointChange += new CounterControls.ScannerFileViewerPanel.StartStopPointChangeHandler (this.ScannerFilePanel_StartStopPointChange);
      // 
      // Auditor
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.SystemColors.Control;
      this.ClientSize = new System.Drawing.Size (984, 762);
      this.Controls.Add (this.ValidateByClassButton);
      this.Controls.Add (this.ScannerFileScrollBar);
      this.Controls.Add (this.RunRecountButton);
      this.Controls.Add (this.GenerateReportButton);
      this.Controls.Add (this.ScannerDisplayControls);
      this.Controls.Add (this.ScannerFileLenInScanLinesLabel);
      this.Controls.Add (this.ScannerFileLenInScanLines);
      this.Controls.Add (this.PlotOptionsLabel);
      this.Controls.Add (this.PlotOptions);
      this.Controls.Add (this.Description);
      this.Controls.Add (this.DescriptionLabel);
      this.Controls.Add (this.ScannerFileLenInSecs);
      this.Controls.Add (this.ScannerFileLenInSecsLabel);
      this.Controls.Add (this.Count);
      this.Controls.Add (this.CountLabel);
      this.Controls.Add (this.ScannerFileChart);
      this.Controls.Add (this.ScannerFilePanel);
      this.Name = "Auditor";
      this.Text = "Auditor";
      this.Load += new System.EventHandler (this.Auditor_Load);
      this.SizeChanged += new System.EventHandler (this.Auditor_SizeChanged);
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler (this.Auditor_FormClosing);
      ((System.ComponentModel.ISupportInitialize)(this.ScannerFileChart)).EndInit ();
      this.ScannerDisplayControls.ResumeLayout (false);
      this.ScannerDisplayControls.PerformLayout ();
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private CounterControls.ScannerFileViewerPanel ScannerFilePanel;
    private System.Windows.Forms.DataVisualization.Charting.Chart ScannerFileChart;
    private System.Windows.Forms.Label CountLabel;
    private System.Windows.Forms.TextBox Count;
    private System.Windows.Forms.Label ScannerFileLenInSecsLabel;
    private System.Windows.Forms.TextBox ScannerFileLenInSecs;
    private System.Windows.Forms.Label DescriptionLabel;
    private System.Windows.Forms.TextBox Description;
    private System.Windows.Forms.HScrollBar ScannerFileScrollBar;
    private System.Windows.Forms.ComboBox PlotOptions;
    private System.Windows.Forms.Label PlotOptionsLabel;
    private System.Windows.Forms.Timer AuditorTimer;
    private System.Windows.Forms.TextBox ScannerFileLenInScanLines;
    private System.Windows.Forms.Label ScannerFileLenInScanLinesLabel;
    private System.Windows.Forms.TextBox AutoScrollingRate;
    private System.Windows.Forms.Button FastForwardButton;
    private System.Windows.Forms.Button FastBackwardButon;
    private System.Windows.Forms.Panel ScannerDisplayControls;
    private System.Windows.Forms.Button StopAutoScrol;
    private System.Windows.Forms.CheckBox Colorize;
    private System.Windows.Forms.Button GenerateReportButton;
    private System.Windows.Forms.Button RunRecountButton;
    private System.Windows.Forms.Button SkipToNextButton;
    private System.Windows.Forms.Button SkipToPrevButton;
    private System.Windows.Forms.ToolTip auditorToolTip;
    private System.Windows.Forms.Button ValidateByClassButton;
  }
}