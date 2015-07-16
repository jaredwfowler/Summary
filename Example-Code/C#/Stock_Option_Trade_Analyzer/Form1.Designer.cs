namespace Stock_Option_Trade_Analyzer
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
         System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
         System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
         System.Windows.Forms.DataVisualization.Charting.Title title1 = new System.Windows.Forms.DataVisualization.Charting.Title();
         this.menuStrip1 = new System.Windows.Forms.MenuStrip();
         this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
         this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
         this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
         this.loadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
         this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
         this.quitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
         this.logInToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
         this.toolStripMenuItem_export = new System.Windows.Forms.ToolStripMenuItem();
         this.toolStripMenuItem_update = new System.Windows.Forms.ToolStripMenuItem();
         this.addToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
         this.toolStripTextBox1 = new System.Windows.Forms.ToolStripTextBox();
         this.chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
         this.dataGridView1 = new System.Windows.Forms.DataGridView();
         this.grid_symbol = new System.Windows.Forms.DataGridViewTextBoxColumn();
         this.grid_name = new System.Windows.Forms.DataGridViewTextBoxColumn();
         this.grid_fidelityScore = new System.Windows.Forms.DataGridViewTextBoxColumn();
         this.grid_graph = new System.Windows.Forms.DataGridViewCheckBoxColumn();
         this.grid_status = new System.Windows.Forms.DataGridViewTextBoxColumn();
         this.grid_remove = new System.Windows.Forms.DataGridViewButtonColumn();
         this.menuStrip1.SuspendLayout();
         ((System.ComponentModel.ISupportInitialize)(this.chart1)).BeginInit();
         ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
         this.SuspendLayout();
         // 
         // menuStrip1
         // 
         this.menuStrip1.BackColor = System.Drawing.SystemColors.HotTrack;
         this.menuStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
         this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.logInToolStripMenuItem,
            this.toolStripMenuItem_export,
            this.toolStripMenuItem_update,
            this.addToolStripMenuItem,
            this.toolStripTextBox1});
         this.menuStrip1.Location = new System.Drawing.Point(0, 0);
         this.menuStrip1.Name = "menuStrip1";
         this.menuStrip1.Size = new System.Drawing.Size(1782, 31);
         this.menuStrip1.TabIndex = 3;
         this.menuStrip1.Text = "menuStrip1";
         // 
         // fileToolStripMenuItem
         // 
         this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            this.loadToolStripMenuItem,
            this.aboutToolStripMenuItem,
            this.quitToolStripMenuItem});
         this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
         this.fileToolStripMenuItem.Size = new System.Drawing.Size(44, 27);
         this.fileToolStripMenuItem.Text = "File";
         // 
         // newToolStripMenuItem
         // 
         this.newToolStripMenuItem.Name = "newToolStripMenuItem";
         this.newToolStripMenuItem.Size = new System.Drawing.Size(134, 24);
         this.newToolStripMenuItem.Text = "New...";
         this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
         // 
         // saveAsToolStripMenuItem
         // 
         this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
         this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(134, 24);
         this.saveAsToolStripMenuItem.Text = "SaveAs...";
         this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
         // 
         // loadToolStripMenuItem
         // 
         this.loadToolStripMenuItem.Name = "loadToolStripMenuItem";
         this.loadToolStripMenuItem.Size = new System.Drawing.Size(134, 24);
         this.loadToolStripMenuItem.Text = "Load";
         this.loadToolStripMenuItem.Click += new System.EventHandler(this.loadToolStripMenuItem_Click);
         // 
         // aboutToolStripMenuItem
         // 
         this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
         this.aboutToolStripMenuItem.Size = new System.Drawing.Size(134, 24);
         this.aboutToolStripMenuItem.Text = "About";
         this.aboutToolStripMenuItem.Click += new System.EventHandler(this.aboutToolStripMenuItem_Click);
         // 
         // quitToolStripMenuItem
         // 
         this.quitToolStripMenuItem.Name = "quitToolStripMenuItem";
         this.quitToolStripMenuItem.Size = new System.Drawing.Size(134, 24);
         this.quitToolStripMenuItem.Text = "Quit";
         this.quitToolStripMenuItem.Click += new System.EventHandler(this.quitToolStripMenuItem_Click);
         // 
         // logInToolStripMenuItem
         // 
         this.logInToolStripMenuItem.Name = "logInToolStripMenuItem";
         this.logInToolStripMenuItem.Size = new System.Drawing.Size(58, 27);
         this.logInToolStripMenuItem.Text = "LogIn";
         this.logInToolStripMenuItem.Click += new System.EventHandler(this.logInToolStripMenuItem_Click);
         // 
         // toolStripMenuItem_export
         // 
         this.toolStripMenuItem_export.Name = "toolStripMenuItem_export";
         this.toolStripMenuItem_export.Size = new System.Drawing.Size(64, 27);
         this.toolStripMenuItem_export.Text = "Export";
         this.toolStripMenuItem_export.Click += new System.EventHandler(this.toolStripMenuItem_export_Click);
         // 
         // toolStripMenuItem_update
         // 
         this.toolStripMenuItem_update.Name = "toolStripMenuItem_update";
         this.toolStripMenuItem_update.Size = new System.Drawing.Size(92, 27);
         this.toolStripMenuItem_update.Text = "Update All";
         this.toolStripMenuItem_update.Click += new System.EventHandler(this.toolStripMenuItem_update_Click);
         // 
         // addToolStripMenuItem
         // 
         this.addToolStripMenuItem.Name = "addToolStripMenuItem";
         this.addToolStripMenuItem.Size = new System.Drawing.Size(49, 27);
         this.addToolStripMenuItem.Text = "Add";
         this.addToolStripMenuItem.Click += new System.EventHandler(this.addToolStripMenuItem_Click);
         // 
         // toolStripTextBox1
         // 
         this.toolStripTextBox1.Name = "toolStripTextBox1";
         this.toolStripTextBox1.Size = new System.Drawing.Size(100, 27);
         this.toolStripTextBox1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.symbolInput_KeyDown);
         this.toolStripTextBox1.KeyUp += new System.Windows.Forms.KeyEventHandler(this.symbolInput_KeyUp);
         // 
         // chart1
         // 
         chartArea1.Name = "ChartArea1";
         this.chart1.ChartAreas.Add(chartArea1);
         legend1.Name = "Legend1";
         this.chart1.Legends.Add(legend1);
         this.chart1.Location = new System.Drawing.Point(726, 50);
         this.chart1.Name = "chart1";
         this.chart1.Palette = System.Windows.Forms.DataVisualization.Charting.ChartColorPalette.None;
         this.chart1.Size = new System.Drawing.Size(1044, 612);
         this.chart1.TabIndex = 1;
         this.chart1.TabStop = false;
         this.chart1.Text = "chart1";
         title1.Name = "Title1";
         title1.Text = "Year-To-Date";
         this.chart1.Titles.Add(title1);
         // 
         // dataGridView1
         // 
         this.dataGridView1.AllowUserToAddRows = false;
         this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
         this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.grid_symbol,
            this.grid_name,
            this.grid_fidelityScore,
            this.grid_graph,
            this.grid_status,
            this.grid_remove});
         this.dataGridView1.Location = new System.Drawing.Point(13, 50);
         this.dataGridView1.Name = "dataGridView1";
         this.dataGridView1.RowTemplate.Height = 24;
         this.dataGridView1.Size = new System.Drawing.Size(684, 612);
         this.dataGridView1.TabIndex = 4;
         this.dataGridView1.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView1_CellContentClick);
         // 
         // grid_symbol
         // 
         this.grid_symbol.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
         this.grid_symbol.HeaderText = "Symbol";
         this.grid_symbol.Name = "grid_symbol";
         this.grid_symbol.ReadOnly = true;
         this.grid_symbol.Resizable = System.Windows.Forms.DataGridViewTriState.False;
         this.grid_symbol.Width = 79;
         // 
         // grid_name
         // 
         this.grid_name.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
         this.grid_name.HeaderText = "Name";
         this.grid_name.Name = "grid_name";
         this.grid_name.ReadOnly = true;
         this.grid_name.Width = 70;
         // 
         // grid_fidelityScore
         // 
         this.grid_fidelityScore.HeaderText = "FidelityScore";
         this.grid_fidelityScore.Name = "grid_fidelityScore";
         this.grid_fidelityScore.ReadOnly = true;
         this.grid_fidelityScore.Resizable = System.Windows.Forms.DataGridViewTriState.False;
         // 
         // grid_graph
         // 
         this.grid_graph.HeaderText = "Graph";
         this.grid_graph.Name = "grid_graph";
         this.grid_graph.Resizable = System.Windows.Forms.DataGridViewTriState.False;
         // 
         // grid_status
         // 
         this.grid_status.HeaderText = "Status";
         this.grid_status.Name = "grid_status";
         this.grid_status.ReadOnly = true;
         this.grid_status.Resizable = System.Windows.Forms.DataGridViewTriState.False;
         // 
         // grid_remove
         // 
         this.grid_remove.HeaderText = "Remove";
         this.grid_remove.Name = "grid_remove";
         this.grid_remove.ReadOnly = true;
         // 
         // Form1
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(1782, 977);
         this.Controls.Add(this.dataGridView1);
         this.Controls.Add(this.chart1);
         this.Controls.Add(this.menuStrip1);
         this.MainMenuStrip = this.menuStrip1;
         this.Name = "Form1";
         this.Text = "Stock_Analyzer";
         this.menuStrip1.ResumeLayout(false);
         this.menuStrip1.PerformLayout();
         ((System.ComponentModel.ISupportInitialize)(this.chart1)).EndInit();
         ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
         this.ResumeLayout(false);
         this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart1;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.DataGridViewTextBoxColumn grid_symbol;
        private System.Windows.Forms.DataGridViewTextBoxColumn grid_name;
        private System.Windows.Forms.DataGridViewTextBoxColumn grid_fidelityScore;
        private System.Windows.Forms.DataGridViewCheckBoxColumn grid_graph;
        private System.Windows.Forms.DataGridViewTextBoxColumn grid_status;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem_export;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem_update;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem loadToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem quitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addToolStripMenuItem;
        private System.Windows.Forms.DataGridViewButtonColumn grid_remove;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem logInToolStripMenuItem;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBox1;
        
    }
}

