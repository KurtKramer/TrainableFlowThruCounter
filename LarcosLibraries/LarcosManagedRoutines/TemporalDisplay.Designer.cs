namespace LarcosManagedRoutines
{
  partial class TemporalDisplay
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
      this.ColIntensityChart = new System.Windows.Forms.DataVisualization.Charting.Chart ();
      this.ColLabel = new System.Windows.Forms.Label ();
      this.Column = new System.Windows.Forms.NumericUpDown ();
      this.StdDevLabel = new System.Windows.Forms.Label ();
      this.MeanLabel = new System.Windows.Forms.Label ();
      this.Mean = new System.Windows.Forms.TextBox ();
      this.StdDev = new System.Windows.Forms.TextBox ();
      this.AvgStdDev = new System.Windows.Forms.TextBox ();
      this.AvgStdDevLabel = new System.Windows.Forms.Label ();
      ((System.ComponentModel.ISupportInitialize)(this.ColIntensityChart)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.Column)).BeginInit ();
      this.SuspendLayout ();
      // 
      // ColIntensityChart
      // 
      chartArea1.AxisX.MajorGrid.Interval = 20;
      chartArea1.Name = "ChartArea1";
      this.ColIntensityChart.ChartAreas.Add (chartArea1);
      legend1.Enabled = false;
      legend1.Name = "Legend1";
      this.ColIntensityChart.Legends.Add (legend1);
      this.ColIntensityChart.Location = new System.Drawing.Point (13, 13);
      this.ColIntensityChart.Name = "ColIntensityChart";
      series1.ChartArea = "ChartArea1";
      series1.Legend = "Legend1";
      series1.Name = "Series1";
      this.ColIntensityChart.Series.Add (series1);
      this.ColIntensityChart.Size = new System.Drawing.Size (872, 431);
      this.ColIntensityChart.TabIndex = 0;
      this.ColIntensityChart.Text = "chart1";
      // 
      // ColLabel
      // 
      this.ColLabel.AutoSize = true;
      this.ColLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ColLabel.Location = new System.Drawing.Point (55, 466);
      this.ColLabel.Name = "ColLabel";
      this.ColLabel.Size = new System.Drawing.Size (53, 16);
      this.ColLabel.TabIndex = 1;
      this.ColLabel.Text = "Column";
      // 
      // Column
      // 
      this.Column.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Column.Location = new System.Drawing.Point (109, 462);
      this.Column.Maximum = new decimal (new int[] {
            2047,
            0,
            0,
            0});
      this.Column.Name = "Column";
      this.Column.Size = new System.Drawing.Size (120, 26);
      this.Column.TabIndex = 2;
      this.Column.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.Column.ValueChanged += new System.EventHandler (this.Column_ValueChanged);
      // 
      // StdDevLabel
      // 
      this.StdDevLabel.AutoSize = true;
      this.StdDevLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.StdDevLabel.Location = new System.Drawing.Point (455, 466);
      this.StdDevLabel.Name = "StdDevLabel";
      this.StdDevLabel.Size = new System.Drawing.Size (53, 16);
      this.StdDevLabel.TabIndex = 3;
      this.StdDevLabel.Text = "StdDev";
      // 
      // MeanLabel
      // 
      this.MeanLabel.AutoSize = true;
      this.MeanLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.MeanLabel.Location = new System.Drawing.Point (290, 466);
      this.MeanLabel.Name = "MeanLabel";
      this.MeanLabel.Size = new System.Drawing.Size (42, 16);
      this.MeanLabel.TabIndex = 4;
      this.MeanLabel.Text = "Mean";
      this.MeanLabel.Click += new System.EventHandler (this.label1_Click);
      // 
      // Mean
      // 
      this.Mean.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Mean.Location = new System.Drawing.Point (335, 462);
      this.Mean.Name = "Mean";
      this.Mean.Size = new System.Drawing.Size (100, 26);
      this.Mean.TabIndex = 5;
      this.Mean.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // StdDev
      // 
      this.StdDev.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.StdDev.Location = new System.Drawing.Point (509, 462);
      this.StdDev.Name = "StdDev";
      this.StdDev.Size = new System.Drawing.Size (100, 26);
      this.StdDev.TabIndex = 6;
      this.StdDev.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // AvgStdDev
      // 
      this.AvgStdDev.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AvgStdDev.Location = new System.Drawing.Point (726, 462);
      this.AvgStdDev.Name = "AvgStdDev";
      this.AvgStdDev.Size = new System.Drawing.Size (100, 26);
      this.AvgStdDev.TabIndex = 8;
      this.AvgStdDev.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // AvgStdDevLabel
      // 
      this.AvgStdDevLabel.AutoSize = true;
      this.AvgStdDevLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AvgStdDevLabel.Location = new System.Drawing.Point (643, 466);
      this.AvgStdDevLabel.Name = "AvgStdDevLabel";
      this.AvgStdDevLabel.Size = new System.Drawing.Size (80, 16);
      this.AvgStdDevLabel.TabIndex = 7;
      this.AvgStdDevLabel.Text = "Avg StdDev";
      // 
      // TemporalDisplay
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (897, 524);
      this.Controls.Add (this.AvgStdDev);
      this.Controls.Add (this.AvgStdDevLabel);
      this.Controls.Add (this.StdDev);
      this.Controls.Add (this.Mean);
      this.Controls.Add (this.MeanLabel);
      this.Controls.Add (this.StdDevLabel);
      this.Controls.Add (this.Column);
      this.Controls.Add (this.ColLabel);
      this.Controls.Add (this.ColIntensityChart);
      this.MinimumSize = new System.Drawing.Size (600, 400);
      this.Name = "TemporalDisplay";
      this.Text = "TemporalDisplay";
      this.Load += new System.EventHandler (this.TemporalDisplay_Load);
      this.SizeChanged += new System.EventHandler (this.TemporalDisplay_SizeChanged);
      ((System.ComponentModel.ISupportInitialize)(this.ColIntensityChart)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.Column)).EndInit ();
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.DataVisualization.Charting.Chart ColIntensityChart;
    private System.Windows.Forms.Label ColLabel;
    private System.Windows.Forms.NumericUpDown Column;
    private System.Windows.Forms.Label StdDevLabel;
    private System.Windows.Forms.Label MeanLabel;
    private System.Windows.Forms.TextBox Mean;
    private System.Windows.Forms.TextBox StdDev;
    private System.Windows.Forms.TextBox AvgStdDev;
    private System.Windows.Forms.Label AvgStdDevLabel;
  }
}