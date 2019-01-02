namespace CounterApplication
{
  using System.Windows.Forms.DataVisualization.Charting;
  partial class FinaleReportViewer
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
      Microsoft.Reporting.WinForms.ReportDataSource reportDataSource1 = new Microsoft.Reporting.WinForms.ReportDataSource();
      Microsoft.Reporting.WinForms.ReportDataSource reportDataSource2 = new Microsoft.Reporting.WinForms.ReportDataSource();
      this.FinaleReportDataSourceBindingSource = new System.Windows.Forms.BindingSource(this.components);
      this.chartsBindingSource = new System.Windows.Forms.BindingSource(this.components);
      this.reportViewer1 = new Microsoft.Reporting.WinForms.ReportViewer();
      ((System.ComponentModel.ISupportInitialize)(this.FinaleReportDataSourceBindingSource)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.chartsBindingSource)).BeginInit();
      this.SuspendLayout();
      // 
      // FinaleReportDataSourceBindingSource
      // 
      this.FinaleReportDataSourceBindingSource.CurrentChanged += new System.EventHandler(this.FinaleReportDataSourceBindingSource_CurrentChanged);
      // 
      // chartsBindingSource
      // 
      this.chartsBindingSource.DataMember = "Charts";
      this.chartsBindingSource.DataSource = this.FinaleReportDataSourceBindingSource;
      this.chartsBindingSource.CurrentChanged += new System.EventHandler(this.chartsBindingSource_CurrentChanged);
      // 
      // reportViewer1
      // 
      this.reportViewer1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      reportDataSource1.Name = "FinaleReportDataSource";
      reportDataSource1.Value = this.FinaleReportDataSourceBindingSource;
      reportDataSource2.Name = "CounterApplication_FinaleReportChartDataSource";
      reportDataSource2.Value = this.chartsBindingSource;
      this.reportViewer1.LocalReport.DataSources.Add(reportDataSource1);
      this.reportViewer1.LocalReport.DataSources.Add(reportDataSource2);
      this.reportViewer1.LocalReport.ReportEmbeddedResource = "CounterApplication.CounterSessionReport.rdlc";
      this.reportViewer1.Location = new System.Drawing.Point(2, 2);
      this.reportViewer1.Name = "reportViewer1";
      this.reportViewer1.Size = new System.Drawing.Size(700, 700);
      this.reportViewer1.TabIndex = 0;
      this.reportViewer1.ZoomMode = Microsoft.Reporting.WinForms.ZoomMode.PageWidth;
      // 
      // FinaleReportViewer
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(708, 705);
      this.Controls.Add(this.reportViewer1);
      this.Name = "FinaleReportViewer";
      this.Text = "FinaleReportViewer";
      this.Load += new System.EventHandler(this.FinaleReportViewer_Load);
      this.SizeChanged += new System.EventHandler(this.FinaleReportViewer_SizeChanged);
      ((System.ComponentModel.ISupportInitialize)(this.FinaleReportDataSourceBindingSource)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.chartsBindingSource)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.BindingSource FinaleReportDataSourceBindingSource;
    private Microsoft.Reporting.WinForms.ReportViewer reportViewer1;
    private System.Windows.Forms.BindingSource chartsBindingSource;
  }
}