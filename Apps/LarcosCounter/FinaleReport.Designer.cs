namespace  LarcosCounter
{
  partial class FinaleReport
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
      System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea ();
      System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend ();
      System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series ();
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint1 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (0, 20);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint2 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (1, 40);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint3 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (2, 67);
      System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea ();
      System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend ();
      System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series ();
      this.CountsChart = new System.Windows.Forms.DataVisualization.Charting.Chart ();
      this.TimeIntervals = new System.Windows.Forms.NumericUpDown ();
      this.TimeIntervalLabel = new System.Windows.Forms.Label ();
      this.TotalCount = new System.Windows.Forms.TextBox ();
      this.TotalCountlabel = new System.Windows.Forms.Label ();
      this.SizeDistributionChart = new System.Windows.Forms.DataVisualization.Charting.Chart ();
      this.ControlNumField = new System.Windows.Forms.TextBox ();
      this.ControlNumLabel = new System.Windows.Forms.Label ();
      this.SessionDescriptionLabel = new System.Windows.Forms.Label ();
      this.SessionDescription2 = new System.Windows.Forms.TextBox ();
      this.SizeInterval = new System.Windows.Forms.NumericUpDown ();
      this.SizeIntervalLabel = new System.Windows.Forms.Label ();
      this.PlotButton = new System.Windows.Forms.Button ();
      this.TrainingModelName2 = new System.Windows.Forms.TextBox ();
      this.TrainingModelNameLabel = new System.Windows.Forms.Label ();
      this.ToLabel = new System.Windows.Forms.Label ();
      this.destination = new System.Windows.Forms.TextBox ();
      this.SourceLabel = new System.Windows.Forms.Label ();
      this.source = new System.Windows.Forms.TextBox ();
      this.CommentsLabel = new System.Windows.Forms.Label ();
      this.comments = new System.Windows.Forms.TextBox ();
      this.PersonInChargeLabel = new System.Windows.Forms.Label ();
      this.personInCharge = new System.Windows.Forms.TextBox ();
      this.ChartTabs = new System.Windows.Forms.TabControl ();
      this.SizeDistributionPage = new System.Windows.Forms.TabPage ();
      this.CountsPage = new System.Windows.Forms.TabPage ();
      this.RecordDateTimeLabel = new System.Windows.Forms.Label ();
      this.recordDateTime = new System.Windows.Forms.TextBox ();
      this.PrintButton = new System.Windows.Forms.Button ();
      this.SizeMeanlabel = new System.Windows.Forms.Label ();
      this.SizeMean = new System.Windows.Forms.TextBox ();
      this.SizeStdDev = new System.Windows.Forms.TextBox ();
      this.SizeStdDevLabel = new System.Windows.Forms.Label ();
      this.SampleSizeLabel = new System.Windows.Forms.Label ();
      this.SampleSize = new System.Windows.Forms.TextBox ();
      ((System.ComponentModel.ISupportInitialize)(this.CountsChart)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.TimeIntervals)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.SizeDistributionChart)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.SizeInterval)).BeginInit ();
      this.ChartTabs.SuspendLayout ();
      this.SizeDistributionPage.SuspendLayout ();
      this.CountsPage.SuspendLayout ();
      this.SuspendLayout ();
      // 
      // CountsChart
      // 
      this.CountsChart.BorderlineColor = System.Drawing.Color.Black;
      this.CountsChart.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      chartArea1.AxisX.Minimum = 0;
      chartArea1.AxisX.Title = "Time";
      chartArea1.AxisY.Title = "Count";
      chartArea1.AxisY2.MajorGrid.Enabled = false;
      chartArea1.Name = "ChartArea1";
      this.CountsChart.ChartAreas.Add (chartArea1);
      legend1.DockedToChartArea = "ChartArea1";
      legend1.Name = "Legend1";
      this.CountsChart.Legends.Add (legend1);
      this.CountsChart.Location = new System.Drawing.Point (3, 3);
      this.CountsChart.Name = "CountsChart";
      series1.ChartArea = "ChartArea1";
      series1.Legend = "Legend1";
      series1.LegendText = "Shrimp Count";
      series1.Name = "Series1";
      dataPoint1.AxisLabel = "00:00";
      dataPoint1.IsValueShownAsLabel = false;
      dataPoint1.Label = "";
      dataPoint2.AxisLabel = "00:10";
      dataPoint2.Label = "";
      dataPoint3.AxisLabel = "00:20";
      series1.Points.Add (dataPoint1);
      series1.Points.Add (dataPoint2);
      series1.Points.Add (dataPoint3);
      this.CountsChart.Series.Add (series1);
      this.CountsChart.Size = new System.Drawing.Size (1083, 484);
      this.CountsChart.TabIndex = 0;
      this.CountsChart.Text = "chart1";
      // 
      // TimeIntervals
      // 
      this.TimeIntervals.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TimeIntervals.Location = new System.Drawing.Point (13, 231);
      this.TimeIntervals.Maximum = new decimal (new int[] {
            600,
            0,
            0,
            0});
      this.TimeIntervals.Minimum = new decimal (new int[] {
            1,
            0,
            0,
            0});
      this.TimeIntervals.Name = "TimeIntervals";
      this.TimeIntervals.Size = new System.Drawing.Size (80, 26);
      this.TimeIntervals.TabIndex = 10;
      this.TimeIntervals.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.TimeIntervals.Value = new decimal (new int[] {
            60,
            0,
            0,
            0});
      // 
      // TimeIntervalLabel
      // 
      this.TimeIntervalLabel.AutoSize = true;
      this.TimeIntervalLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TimeIntervalLabel.Location = new System.Drawing.Point (9, 212);
      this.TimeIntervalLabel.Name = "TimeIntervalLabel";
      this.TimeIntervalLabel.Size = new System.Drawing.Size (85, 16);
      this.TimeIntervalLabel.TabIndex = 6;
      this.TimeIntervalLabel.Text = "Time Interval";
      // 
      // TotalCount
      // 
      this.TotalCount.Font = new System.Drawing.Font ("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TotalCount.Location = new System.Drawing.Point (982, 35);
      this.TotalCount.Name = "TotalCount";
      this.TotalCount.ReadOnly = true;
      this.TotalCount.Size = new System.Drawing.Size (130, 35);
      this.TotalCount.TabIndex = 8;
      this.TotalCount.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // TotalCountlabel
      // 
      this.TotalCountlabel.AutoSize = true;
      this.TotalCountlabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TotalCountlabel.Location = new System.Drawing.Point (1006, 17);
      this.TotalCountlabel.Name = "TotalCountlabel";
      this.TotalCountlabel.Size = new System.Drawing.Size (76, 16);
      this.TotalCountlabel.TabIndex = 9;
      this.TotalCountlabel.Text = "Total Count";
      // 
      // SizeDistributionChart
      // 
      this.SizeDistributionChart.BorderlineColor = System.Drawing.Color.Black;
      this.SizeDistributionChart.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      chartArea2.AxisX.Title = "Size(mm)";
      chartArea2.AxisY.Title = "Count";
      chartArea2.Name = "ChartArea1";
      this.SizeDistributionChart.ChartAreas.Add (chartArea2);
      legend2.Enabled = false;
      legend2.Name = "Legend1";
      this.SizeDistributionChart.Legends.Add (legend2);
      this.SizeDistributionChart.Location = new System.Drawing.Point (3, 3);
      this.SizeDistributionChart.Name = "SizeDistributionChart";
      series2.ChartArea = "ChartArea1";
      series2.Legend = "Legend1";
      series2.Name = "Series1";
      this.SizeDistributionChart.Series.Add (series2);
      this.SizeDistributionChart.Size = new System.Drawing.Size (1083, 484);
      this.SizeDistributionChart.TabIndex = 110;
      this.SizeDistributionChart.Text = "chart2";
      this.SizeDistributionChart.MouseClick += new System.Windows.Forms.MouseEventHandler (this.SizeDistributionChart_MouseClick);
      this.SizeDistributionChart.Click += new System.EventHandler (this.SizeDistributionChart_Click);
      // 
      // ControlNumField
      // 
      this.ControlNumField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ControlNumField.Location = new System.Drawing.Point (12, 35);
      this.ControlNumField.Name = "ControlNumField";
      this.ControlNumField.ReadOnly = true;
      this.ControlNumField.Size = new System.Drawing.Size (125, 26);
      this.ControlNumField.TabIndex = 13;
      // 
      // ControlNumLabel
      // 
      this.ControlNumLabel.AutoSize = true;
      this.ControlNumLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ControlNumLabel.Location = new System.Drawing.Point (15, 17);
      this.ControlNumLabel.Name = "ControlNumLabel";
      this.ControlNumLabel.Size = new System.Drawing.Size (81, 16);
      this.ControlNumLabel.TabIndex = 14;
      this.ControlNumLabel.Text = "Control Num";
      // 
      // SessionDescriptionLabel
      // 
      this.SessionDescriptionLabel.AutoSize = true;
      this.SessionDescriptionLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SessionDescriptionLabel.Location = new System.Drawing.Point (151, 17);
      this.SessionDescriptionLabel.Name = "SessionDescriptionLabel";
      this.SessionDescriptionLabel.Size = new System.Drawing.Size (128, 16);
      this.SessionDescriptionLabel.TabIndex = 16;
      this.SessionDescriptionLabel.Text = "Session Description";
      // 
      // SessionDescription2
      // 
      this.SessionDescription2.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SessionDescription2.Location = new System.Drawing.Point (148, 35);
      this.SessionDescription2.Name = "SessionDescription2";
      this.SessionDescription2.ReadOnly = true;
      this.SessionDescription2.Size = new System.Drawing.Size (559, 26);
      this.SessionDescription2.TabIndex = 15;
      // 
      // SizeInterval
      // 
      this.SizeInterval.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SizeInterval.Location = new System.Drawing.Point (113, 231);
      this.SizeInterval.Minimum = new decimal (new int[] {
            1,
            0,
            0,
            0});
      this.SizeInterval.Name = "SizeInterval";
      this.SizeInterval.Size = new System.Drawing.Size (80, 26);
      this.SizeInterval.TabIndex = 20;
      this.SizeInterval.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.SizeInterval.Value = new decimal (new int[] {
            1,
            0,
            0,
            0});
      // 
      // SizeIntervalLabel
      // 
      this.SizeIntervalLabel.AutoSize = true;
      this.SizeIntervalLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SizeIntervalLabel.Location = new System.Drawing.Point (113, 212);
      this.SizeIntervalLabel.Name = "SizeIntervalLabel";
      this.SizeIntervalLabel.Size = new System.Drawing.Size (80, 16);
      this.SizeIntervalLabel.TabIndex = 18;
      this.SizeIntervalLabel.Text = "Size Interval";
      // 
      // PlotButton
      // 
      this.PlotButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.PlotButton.Location = new System.Drawing.Point (211, 229);
      this.PlotButton.Name = "PlotButton";
      this.PlotButton.Size = new System.Drawing.Size (75, 30);
      this.PlotButton.TabIndex = 30;
      this.PlotButton.Text = "Plot";
      this.PlotButton.UseVisualStyleBackColor = true;
      this.PlotButton.Click += new System.EventHandler (this.PlotButton_Click);
      // 
      // TrainingModelName2
      // 
      this.TrainingModelName2.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TrainingModelName2.Location = new System.Drawing.Point (721, 35);
      this.TrainingModelName2.Name = "TrainingModelName2";
      this.TrainingModelName2.ReadOnly = true;
      this.TrainingModelName2.Size = new System.Drawing.Size (246, 26);
      this.TrainingModelName2.TabIndex = 20;
      // 
      // TrainingModelNameLabel
      // 
      this.TrainingModelNameLabel.AutoSize = true;
      this.TrainingModelNameLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TrainingModelNameLabel.Location = new System.Drawing.Point (724, 17);
      this.TrainingModelNameLabel.Name = "TrainingModelNameLabel";
      this.TrainingModelNameLabel.Size = new System.Drawing.Size (63, 16);
      this.TrainingModelNameLabel.TabIndex = 21;
      this.TrainingModelNameLabel.Text = "Classifier";
      // 
      // ToLabel
      // 
      this.ToLabel.AutoSize = true;
      this.ToLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ToLabel.Location = new System.Drawing.Point (181, 132);
      this.ToLabel.Name = "ToLabel";
      this.ToLabel.Size = new System.Drawing.Size (75, 16);
      this.ToLabel.TabIndex = 304;
      this.ToLabel.Text = "Destination";
      // 
      // destination
      // 
      this.destination.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.destination.Location = new System.Drawing.Point (178, 150);
      this.destination.Name = "destination";
      this.destination.ReadOnly = true;
      this.destination.Size = new System.Drawing.Size (150, 26);
      this.destination.TabIndex = 307;
      this.destination.Text = "Pond-1A";
      // 
      // SourceLabel
      // 
      this.SourceLabel.AutoSize = true;
      this.SourceLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SourceLabel.Location = new System.Drawing.Point (16, 132);
      this.SourceLabel.Name = "SourceLabel";
      this.SourceLabel.Size = new System.Drawing.Size (51, 16);
      this.SourceLabel.TabIndex = 303;
      this.SourceLabel.Text = "Source";
      // 
      // source
      // 
      this.source.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.source.Location = new System.Drawing.Point (12, 151);
      this.source.Name = "source";
      this.source.ReadOnly = true;
      this.source.Size = new System.Drawing.Size (150, 26);
      this.source.TabIndex = 306;
      this.source.Text = "Pool";
      // 
      // CommentsLabel
      // 
      this.CommentsLabel.AutoSize = true;
      this.CommentsLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CommentsLabel.Location = new System.Drawing.Point (386, 75);
      this.CommentsLabel.Name = "CommentsLabel";
      this.CommentsLabel.Size = new System.Drawing.Size (72, 16);
      this.CommentsLabel.TabIndex = 302;
      this.CommentsLabel.Text = "Comments";
      // 
      // comments
      // 
      this.comments.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.comments.Location = new System.Drawing.Point (383, 94);
      this.comments.Multiline = true;
      this.comments.Name = "comments";
      this.comments.ReadOnly = true;
      this.comments.Size = new System.Drawing.Size (729, 83);
      this.comments.TabIndex = 308;
      // 
      // PersonInChargeLabel
      // 
      this.PersonInChargeLabel.AutoSize = true;
      this.PersonInChargeLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.PersonInChargeLabel.Location = new System.Drawing.Point (15, 75);
      this.PersonInChargeLabel.Name = "PersonInChargeLabel";
      this.PersonInChargeLabel.Size = new System.Drawing.Size (111, 16);
      this.PersonInChargeLabel.TabIndex = 301;
      this.PersonInChargeLabel.Text = "Person in Charge";
      // 
      // personInCharge
      // 
      this.personInCharge.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.personInCharge.Location = new System.Drawing.Point (12, 93);
      this.personInCharge.Name = "personInCharge";
      this.personInCharge.ReadOnly = true;
      this.personInCharge.Size = new System.Drawing.Size (246, 26);
      this.personInCharge.TabIndex = 305;
      // 
      // ChartTabs
      // 
      this.ChartTabs.Controls.Add (this.SizeDistributionPage);
      this.ChartTabs.Controls.Add (this.CountsPage);
      this.ChartTabs.Location = new System.Drawing.Point (12, 282);
      this.ChartTabs.Name = "ChartTabs";
      this.ChartTabs.SelectedIndex = 0;
      this.ChartTabs.Size = new System.Drawing.Size (1100, 516);
      this.ChartTabs.TabIndex = 120;
      // 
      // SizeDistributionPage
      // 
      this.SizeDistributionPage.Controls.Add (this.SizeDistributionChart);
      this.SizeDistributionPage.Location = new System.Drawing.Point (4, 22);
      this.SizeDistributionPage.Name = "SizeDistributionPage";
      this.SizeDistributionPage.Padding = new System.Windows.Forms.Padding (3);
      this.SizeDistributionPage.Size = new System.Drawing.Size (1092, 490);
      this.SizeDistributionPage.TabIndex = 0;
      this.SizeDistributionPage.Text = "Size Distribution";
      this.SizeDistributionPage.UseVisualStyleBackColor = true;
      // 
      // CountsPage
      // 
      this.CountsPage.Controls.Add (this.CountsChart);
      this.CountsPage.Location = new System.Drawing.Point (4, 22);
      this.CountsPage.Name = "CountsPage";
      this.CountsPage.Padding = new System.Windows.Forms.Padding (3);
      this.CountsPage.Size = new System.Drawing.Size (1092, 490);
      this.CountsPage.TabIndex = 1;
      this.CountsPage.Text = "Counts by Time";
      this.CountsPage.UseVisualStyleBackColor = true;
      // 
      // RecordDateTimeLabel
      // 
      this.RecordDateTimeLabel.AutoSize = true;
      this.RecordDateTimeLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.RecordDateTimeLabel.Location = new System.Drawing.Point (386, 212);
      this.RecordDateTimeLabel.Name = "RecordDateTimeLabel";
      this.RecordDateTimeLabel.Size = new System.Drawing.Size (120, 16);
      this.RecordDateTimeLabel.TabIndex = 309;
      this.RecordDateTimeLabel.Text = "Record Date-Time";
      // 
      // recordDateTime
      // 
      this.recordDateTime.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.recordDateTime.Location = new System.Drawing.Point (383, 231);
      this.recordDateTime.Name = "recordDateTime";
      this.recordDateTime.ReadOnly = true;
      this.recordDateTime.Size = new System.Drawing.Size (190, 26);
      this.recordDateTime.TabIndex = 310;
      // 
      // PrintButton
      // 
      this.PrintButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.PrintButton.Location = new System.Drawing.Point (1037, 229);
      this.PrintButton.Name = "PrintButton";
      this.PrintButton.Size = new System.Drawing.Size (75, 30);
      this.PrintButton.TabIndex = 311;
      this.PrintButton.Text = "Print";
      this.PrintButton.UseVisualStyleBackColor = true;
      this.PrintButton.Click += new System.EventHandler (this.PrintButton_Click);
      // 
      // SizeMeanlabel
      // 
      this.SizeMeanlabel.AutoSize = true;
      this.SizeMeanlabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SizeMeanlabel.Location = new System.Drawing.Point (755, 212);
      this.SizeMeanlabel.Name = "SizeMeanlabel";
      this.SizeMeanlabel.Size = new System.Drawing.Size (71, 16);
      this.SizeMeanlabel.TabIndex = 312;
      this.SizeMeanlabel.Text = "Mean Size";
      // 
      // SizeMean
      // 
      this.SizeMean.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SizeMean.Location = new System.Drawing.Point (741, 231);
      this.SizeMean.Name = "SizeMean";
      this.SizeMean.ReadOnly = true;
      this.SizeMean.Size = new System.Drawing.Size (100, 26);
      this.SizeMean.TabIndex = 313;
      // 
      // SizeStdDev
      // 
      this.SizeStdDev.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SizeStdDev.Location = new System.Drawing.Point (847, 231);
      this.SizeStdDev.Name = "SizeStdDev";
      this.SizeStdDev.ReadOnly = true;
      this.SizeStdDev.Size = new System.Drawing.Size (57, 26);
      this.SizeStdDev.TabIndex = 314;
      // 
      // SizeStdDevLabel
      // 
      this.SizeStdDevLabel.AutoSize = true;
      this.SizeStdDevLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SizeStdDevLabel.Location = new System.Drawing.Point (847, 212);
      this.SizeStdDevLabel.Name = "SizeStdDevLabel";
      this.SizeStdDevLabel.Size = new System.Drawing.Size (57, 16);
      this.SizeStdDevLabel.TabIndex = 315;
      this.SizeStdDevLabel.Text = "Std-Dev";
      // 
      // SampleSizeLabel
      // 
      this.SampleSizeLabel.AutoSize = true;
      this.SampleSizeLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SampleSizeLabel.Location = new System.Drawing.Point (646, 212);
      this.SampleSizeLabel.Name = "SampleSizeLabel";
      this.SampleSizeLabel.Size = new System.Drawing.Size (84, 16);
      this.SampleSizeLabel.TabIndex = 316;
      this.SampleSizeLabel.Text = "Sample Size";
      // 
      // SampleSize
      // 
      this.SampleSize.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SampleSize.Location = new System.Drawing.Point (635, 231);
      this.SampleSize.Name = "SampleSize";
      this.SampleSize.ReadOnly = true;
      this.SampleSize.Size = new System.Drawing.Size (100, 26);
      this.SampleSize.TabIndex = 317;
      // 
      // FinaleReport
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (1128, 800);
      this.Controls.Add (this.SampleSize);
      this.Controls.Add (this.SampleSizeLabel);
      this.Controls.Add (this.SizeStdDevLabel);
      this.Controls.Add (this.SizeStdDev);
      this.Controls.Add (this.SizeMean);
      this.Controls.Add (this.SizeMeanlabel);
      this.Controls.Add (this.PrintButton);
      this.Controls.Add (this.recordDateTime);
      this.Controls.Add (this.RecordDateTimeLabel);
      this.Controls.Add (this.ChartTabs);
      this.Controls.Add (this.ToLabel);
      this.Controls.Add (this.destination);
      this.Controls.Add (this.SourceLabel);
      this.Controls.Add (this.source);
      this.Controls.Add (this.CommentsLabel);
      this.Controls.Add (this.comments);
      this.Controls.Add (this.PersonInChargeLabel);
      this.Controls.Add (this.personInCharge);
      this.Controls.Add (this.TrainingModelNameLabel);
      this.Controls.Add (this.TrainingModelName2);
      this.Controls.Add (this.PlotButton);
      this.Controls.Add (this.SizeIntervalLabel);
      this.Controls.Add (this.SizeInterval);
      this.Controls.Add (this.SessionDescriptionLabel);
      this.Controls.Add (this.SessionDescription2);
      this.Controls.Add (this.ControlNumLabel);
      this.Controls.Add (this.ControlNumField);
      this.Controls.Add (this.TotalCountlabel);
      this.Controls.Add (this.TotalCount);
      this.Controls.Add (this.TimeIntervalLabel);
      this.Controls.Add (this.TimeIntervals);
      this.Name = "FinaleReport";
      this.Text = "`";
      this.Load += new System.EventHandler (this.FinaleReport_Load);
      this.SizeChanged += new System.EventHandler (this.FinaleReport_SizeChanged);
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler (this.FinaleReport_FormClosing);
      this.Resize += new System.EventHandler (this.FinaleReport_Resize);
      ((System.ComponentModel.ISupportInitialize)(this.CountsChart)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.TimeIntervals)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.SizeDistributionChart)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.SizeInterval)).EndInit ();
      this.ChartTabs.ResumeLayout (false);
      this.SizeDistributionPage.ResumeLayout (false);
      this.CountsPage.ResumeLayout (false);
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.DataVisualization.Charting.Chart CountsChart;
    private System.Windows.Forms.NumericUpDown TimeIntervals;
    private System.Windows.Forms.Label TimeIntervalLabel;
    private System.Windows.Forms.TextBox TotalCount;
    private System.Windows.Forms.Label TotalCountlabel;
    private System.Windows.Forms.DataVisualization.Charting.Chart SizeDistributionChart;
    private System.Windows.Forms.TextBox ControlNumField;
    private System.Windows.Forms.Label ControlNumLabel;
    private System.Windows.Forms.Label SessionDescriptionLabel;
    private System.Windows.Forms.TextBox SessionDescription2;
    private System.Windows.Forms.NumericUpDown SizeInterval;
    private System.Windows.Forms.Label SizeIntervalLabel;
    private System.Windows.Forms.Button PlotButton;
    private System.Windows.Forms.TextBox TrainingModelName2;
    private System.Windows.Forms.Label TrainingModelNameLabel;
    private System.Windows.Forms.Label ToLabel;
    private System.Windows.Forms.TextBox destination;
    private System.Windows.Forms.Label SourceLabel;
    private System.Windows.Forms.TextBox source;
    private System.Windows.Forms.Label CommentsLabel;
    private System.Windows.Forms.TextBox comments;
    private System.Windows.Forms.Label PersonInChargeLabel;
    private System.Windows.Forms.TextBox personInCharge;
    private System.Windows.Forms.TabControl ChartTabs;
    private System.Windows.Forms.TabPage SizeDistributionPage;
    private System.Windows.Forms.TabPage CountsPage;
    private System.Windows.Forms.Label RecordDateTimeLabel;
    private System.Windows.Forms.TextBox recordDateTime;
    private System.Windows.Forms.Button PrintButton;
    private System.Windows.Forms.Label SizeMeanlabel;
    private System.Windows.Forms.TextBox SizeMean;
    private System.Windows.Forms.TextBox SizeStdDev;
    private System.Windows.Forms.Label SizeStdDevLabel;
    private System.Windows.Forms.Label SampleSizeLabel;
    private System.Windows.Forms.TextBox SampleSize;
  }
}